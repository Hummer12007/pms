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

#include "../build.h"
#include "console.h"
#include "curses.h"
#include "config.h"
#include "window.h"
#include "mpd.h"
#include "input.h"
#include "pms.h"
#include "field.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/time.h>

Fieldtypes *	fieldtypes;
Config *	config;
MPD *		mpd;
Windowmanager *	wm;
Input *		input;
PMS *		pms;
extern Keybindings * keybindings;
extern Commandlist * commandlist;

int main(int argc, char *argv[])
{
	struct timeval cl;
	struct timeval conn;
	bool initialized = false;

	setlocale(LC_ALL, "");

	fieldtypes = new Fieldtypes();
	keybindings = new Keybindings();
	commandlist = new Commandlist();
	mpd = new MPD();
	input = new Input();
	pms = new PMS();
	wm = new Windowmanager();
	wm->init_ncurses();
	config = new Config();

	wm->detect_dimensions();
	wm->activate(WMAIN(wm->console));
	stinfo("%s", PACKAGE_STRING);
	config->source_default_config();

	memset(&conn, 0, sizeof conn);
	wm->playlist->songlist = &(mpd->playlist);
	wm->library->songlist = &(mpd->library);
	wm->draw();

	while(!config->quit)
	{
		gettimeofday(&cl, NULL);
		if (!mpd->is_connected() && config->autoconnect)
		{
			if (cl.tv_sec - conn.tv_sec >= (int)(config->reconnect_delay))
			{
				wm->draw();
				if (mpd->mpd_connect(config->host, config->port))
				{
					mpd->set_password(config->password);
					mpd->get_status();
					mpd->get_playlist();
					wm->qdraw();
					mpd->get_library();
					mpd->read_opts();
					mpd->update_playstring();
					if (!initialized)
					{
						initialized = true;
						wm->activate(WMAIN(wm->playlist));
						if (mpd->currentsong)
							wm->active->set_cursor(mpd->currentsong->pos);
						stinfo("Ready.", NULL);
					}
					wm->qdraw();
				}
				gettimeofday(&conn, NULL);
			}
		}

		/* Check if statusbar needs a reset draw */
		memcpy(&(wm->statusbar->cl), &cl, sizeof cl);
		if (wm->statusbar->cl.tv_sec - wm->statusbar->cl_reset.tv_sec >= (int)(config->status_reset_interval))
			wm->statusbar->qdraw();

		/* Get updates from MPD, run clock, do updates */
		wm->qdraw();
		mpd->poll();
		wm->qdraw();

		/* Check for any input events and run them */
		pms->run_event(input->next());
	}

	delete wm;
}
