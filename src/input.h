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

#ifndef _PMS_INPUT_H_
#define _PMS_INPUT_H_

#include "command.h"
#include "config.h"
#include <string>
using namespace std;

#define INPUT_RESULT_NOINPUT 0
#define INPUT_RESULT_BUFFERED 1
#define INPUT_RESULT_MULTIPLIER 2
#define INPUT_RESULT_RUN 3

#define INPUT_MODE_COMMAND 0
#define INPUT_MODE_INPUT 1
#define INPUT_MODE_SEARCH 2
#define INPUT_MODE_LIVESEARCH 3

#define KEYBIND_FIND_NOMATCH -1
#define KEYBIND_FIND_EXACT 0
#define KEYBIND_FIND_BUFFERED 1

/* This class is returned by Input::next(), defining what to do next */
class Inputevent
{
	public:
		Inputevent();
		void clear();

		Inputevent & operator= (const Inputevent & src);

		/* Which context are we in? */
		int context;

		/* How many? */
		unsigned int multiplier;

		/* What kind of action to run */
		action_t action;

		/* one of INPUT_RESULT_* */
		int result;

		/* Shut up about any messages */
		bool silent;

		/* The full character buffer or command/input */
		string text;
};

class Keybinding
{
	public:
		string		seqstr;
		vector<int>	sequence;
		action_t	action;
		int		context;
		string		params;
};

class Keybindings
{
	private:
		vector<Keybinding *>	bindings;

	public:
		void		load_defaults();

		/* Add and check for duplicate sequences */
		Keybinding *	add(int context, action_t action, string sequence, string params = "");
		Keybinding *	find_conflict(vector<int> * sequence);

		/* Delete a mapping */
		bool		remove(string sequence);

		/* Convert a string sequence to an int sequence */
		vector<int> *	conv_sequence(string seq);

		/* Find an action based on the key sequence */
		int		find(int context, vector<int> * sequence, action_t * action, string * params);

		/* Delete all mappings */
		void		truncate();
};

class Input
{
	private:
		/* Used for conversion from/to wide character */
		char			mbs_buffer[1024];
		wchar_t			wcs_buffer[1024];

		wint_t			chbuf;
		bool			is_tab_completing;
		bool			is_option_tab_completing;
		string			option_tab_prefix;
		unsigned int		tab_complete_index;
		unsigned int		option_tab_complete_index;
		vector<Command *> *	tab_results;
		vector<option_t *> 	option_tab_results;
		Inputevent		ev;

		void			handle_text_input();

		/* Translate input event so that keys are handled the same on different terminals */
		void			tr_chbuf();

		/* Check if input event is a number, and apply multiplier */
		bool			run_multiplier(int ch);

	public:

		unsigned int	cursorpos;
		int		mode;
		unsigned long	multiplier;
		vector<int> 	buffer;
		wstring		wstrbuf;
		string		strbuf;

		Input();

		/* Read next character from ncurses buffer */
		Inputevent *	next();

		/* Convert from/to wide-string */
		size_t		conv_to_mbs();
		size_t		conv_to_wcs();

		/* Setter and getter for mode */
		void		setmode(int nmode);
		int		getmode() { return mode; }
};

#endif /* _PMS_INPUT_H_ */
