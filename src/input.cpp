/* vi:set ts=8 sts=8 sw=8 noet:
 *
 * Practical Music Search
 * Copyright (c) 2006-2011  Kim Tore Jensen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "input.h"
#include "command.h"
#include "window.h"
#include "config.h"
#include "console.h"
#include <cstring>
#include <stdlib.h>

Keybindings * keybindings;
Commandlist * commandlist;
extern Windowmanager * wm;
extern Config * config;

Inputevent::Inputevent()
{
	clear();
}

void Inputevent::clear()
{
	context = 0;
	multiplier = 1;
	action = ACT_NOACTION;
	result = INPUT_RESULT_NOINPUT;
	context = 0;
	silent = false;
	text.clear();
}

Inputevent & Inputevent::operator= (const Inputevent & src)
{
	if (this == &src)
		return *this;

	clear();
	result = src.result;
	context = src.context;
	multiplier = src.multiplier;
	action = src.action;
	context = src.context;
	text = src.text;

	return *this;
}

Input::Input()
{
	mode = INPUT_MODE_COMMAND;
	chbuf = 0;
	multiplier = 0;
	wstrbuf.clear();
	buffer.clear();
	is_tab_completing = false;
	is_option_tab_completing = false;
	option_tab_prefix.clear();
	tab_complete_index = 0;
	option_tab_complete_index = 0;
	cursorpos = 0;
}

bool Input::run_multiplier(int ch)
{
	if (ch < '0' || ch > '9')
		return false;
	
	multiplier = (multiplier * 10) + ch - 48;
	return true;
}

Inputevent * Input::next()
{
	int m;

	get_wch(&chbuf);
	if (chbuf == ERR)
	{
		return NULL;
	}

	ev.clear();

	/* This is a global signal/event not dependant on anything else. */
	if (chbuf == KEY_RESIZE)
	{
		ev.result = INPUT_RESULT_RUN;
		ev.action = ACT_RESIZE;
		return &ev;
	}
	tr_chbuf();

	switch(mode)
	{
		/* Command-mode */
		default:
		case INPUT_MODE_COMMAND:
			if (run_multiplier(chbuf))
			{
				ev.result = INPUT_RESULT_MULTIPLIER;
				break;
			}

			buffer.push_back(chbuf);
			wstrbuf.push_back(chbuf);
			conv_to_mbs();
			m = keybindings->find(wm->context, &buffer, &ev.action, &strbuf);

			if (m == KEYBIND_FIND_EXACT)
				ev.result = INPUT_RESULT_RUN;
			else if (m == KEYBIND_FIND_BUFFERED)
				ev.result = INPUT_RESULT_BUFFERED;
			else if (m == KEYBIND_FIND_NOMATCH)
			{
				buffer.clear();
				wstrbuf.clear();
				multiplier = 0;
			}

			break;

		/* Text input of some sorts */
		case INPUT_MODE_INPUT:
		case INPUT_MODE_SEARCH:
		case INPUT_MODE_LIVESEARCH:
			handle_text_input();
			break;
	}

	if (ev.result != INPUT_RESULT_NOINPUT)
	{
		conv_to_mbs();
		ev.context = wm->context;
		ev.text = strbuf;
		ev.multiplier = multiplier > 0 ? multiplier : 1;

		if (ev.result != INPUT_RESULT_BUFFERED && ev.result != INPUT_RESULT_MULTIPLIER)
		{
			buffer.clear();
			wstrbuf.clear();
			if (ev.action != ACT_MODE_INPUT)
				multiplier = 0;
		}

		return &ev;
	}
	
	return NULL;
}

void Input::tr_chbuf()
{
	//int old = chbuf;
	switch(chbuf)
	{
		case 8:			/* ^H -- backspace */
		case 127:		/* ^? -- delete */
			chbuf = KEY_BACKSPACE;
			break;

		case 10:		/* Return */
		case 343:		/* Enter (keypad) */
			chbuf = KEY_ENTER;
			break;

		default:
			return;
	}
	//debug("Warning: translating keycode %d to %d", old, chbuf);
}

void Input::handle_text_input()
{
	option_t * opt;
	wstring::iterator si;
	size_t fpos;
	size_t pos;

	if (chbuf != 9)
	{
		is_tab_completing = false;
		is_option_tab_completing = false;
	}

	switch(chbuf)
	{
		/* Navigation */
		case KEY_LEFT:
			if (cursorpos > 0)
				--cursorpos;
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		case KEY_RIGHT:
			if (cursorpos < wstrbuf.size())
				++cursorpos;
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		case 1:			/* ^A */
		case KEY_HOME:
			cursorpos = 0;
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		case 5:			/* ^E */
		case KEY_END:
			cursorpos = wstrbuf.size();
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		case 10:
		case KEY_ENTER:
			if (mode == INPUT_MODE_INPUT)
				ev.action = ACT_RUN_CMD;
			else if (mode == INPUT_MODE_SEARCH)
				ev.action = ACT_RUN_SEARCH;
			else if (mode == INPUT_MODE_LIVESEARCH)
				ev.action = ACT_EXIT_LIVESEARCH;

			ev.result = INPUT_RESULT_RUN;
			return;

		case 21:		/* ^U */
			if (cursorpos > 0)
			{
				buffer.erase(vector<int>::iterator(buffer.begin()), vector<int>::iterator(buffer.begin() + cursorpos));
				wstrbuf.erase(wstring::iterator(wstrbuf.begin()), wstring::iterator(wstrbuf.begin() + cursorpos));
			}
			cursorpos = 0;
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		case 27:		/* Escape */
			ev.result = INPUT_RESULT_RUN;
			ev.action = ACT_MODE_COMMAND;
			return;

		case KEY_BACKSPACE:
			if (buffer.size() > 0)
			{
				if (cursorpos > 0)
				{
					buffer.erase(--vector<int>::iterator(buffer.begin() + cursorpos));
					wstrbuf.erase(--wstring::iterator(wstrbuf.begin() + cursorpos));
					if (cursorpos > 0)
						--cursorpos;
				}
				ev.result = INPUT_RESULT_BUFFERED;
			}
			else
			{
				ev.result = INPUT_RESULT_RUN;
				ev.action = ACT_MODE_COMMAND;
			}
			return;

		case 9:			/* Tab */
			/* Tabcomplete options instead of commands */
			if ((wstrbuf.size() >= 3 && wstrbuf.substr(0, 3) == L"se ") ||
				(wstrbuf.size() >= 4 && wstrbuf.substr(0, 4) == L"set "))
			{
				fpos = wstrbuf.find(L' ') + 1;

				/* No equal sign given, cycle through options */
				if ((pos = wstrbuf.find(L'=', fpos)) == wstring::npos)
				{
					if (is_option_tab_completing)
					{
						if (++option_tab_complete_index >= option_tab_results.size())
							option_tab_complete_index = 0;
					}
					else
					{
						conv_to_mbs();
						config->grep_opt(strbuf.substr(fpos), &option_tab_results, &option_tab_prefix);
						if (option_tab_results.size() > 0)
						{
							is_option_tab_completing = true;
							option_tab_complete_index = 0;
						}
					}

					if (is_option_tab_completing)
					{
						if ((opt = option_tab_results[option_tab_complete_index]) != NULL)
						{
							conv_to_mbs();
							strbuf = strbuf.substr(0, fpos) + option_tab_prefix + opt->name;
							conv_to_wcs();
						}
					}
				}

				/* Equal sign found, print option if none given. */
				else if (pos + 1 == wstrbuf.size())
				{
					conv_to_mbs();
					opt = config->get_opt_ptr(strbuf.substr(fpos, pos - fpos));
					if (opt && opt->type != OPTION_TYPE_BOOL)
					{
						strbuf = strbuf + config->get_opt_str(opt);
						conv_to_wcs();
					}
				}

			}

			/* Tabcomplete commands */
			else
			{
				if (is_tab_completing)
				{
					if (++tab_complete_index >= tab_results->size())
						tab_complete_index = 0;
				}
				else
				{
					conv_to_mbs();
					tab_results = commandlist->grep(wm->context, strbuf);
					if (tab_results->size() > 0)
					{
						is_tab_completing = true;
						tab_complete_index = 0;
					}
				}

				if (is_tab_completing)
				{
					conv_to_mbs();
					strbuf = tab_results->at(tab_complete_index)->name;
					conv_to_wcs();
				}
			}

			/* Sync binary input buffer with string buffer */
			buffer.clear();
			for (si = wstrbuf.begin(); si != wstrbuf.end(); ++si)
				buffer.push_back(*si);

			cursorpos = buffer.size();
			ev.result = INPUT_RESULT_BUFFERED;
			return;

		/* Add normal character to buffer */
		default:
			/* Ignore key codes */
			if (chbuf < 32 || chbuf >= KEY_CODE_YES)
				return;

			buffer.insert(vector<int>::iterator(buffer.begin() + cursorpos), chbuf);
			wstrbuf.insert(wstring::iterator(wstrbuf.begin() + cursorpos), chbuf);
			++cursorpos;
			ev.result = INPUT_RESULT_BUFFERED;
	}
}

void Input::setmode(int nmode)
{
	if (nmode == mode)
		return;
	
	wstrbuf.clear();
	buffer.clear();
	chbuf = 0;
	cursorpos = 0;
	mode = nmode;

	if (mode == INPUT_MODE_COMMAND)
		curs_set(0);
	else
		curs_set(1);
}

size_t Input::conv_to_mbs()
{
	size_t r = wcstombs(mbs_buffer, wstrbuf.c_str(), 1024);
	strbuf = mbs_buffer;
	return r;
}

size_t Input::conv_to_wcs()
{
	size_t r = mbstowcs(wcs_buffer, strbuf.c_str(), 1024);
	wstrbuf = wcs_buffer;
	return r;
}
