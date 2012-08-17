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

#ifndef _PMS_WINDOW_H_
#define _PMS_WINDOW_H_

#include "../build.h"

#if defined HAVE_NCURSESW_CURSES_H
	#include <ncursesw/curses.h>
#elif defined HAVE_NCURSESW_H
	#include <ncursesw.h>
#elif defined HAVE_NCURSES_CURSES_H
	#include <ncurses/curses.h>
#elif defined HAVE_NCURSES_H
	#include <ncurses.h>
#elif defined HAVE_CURSES_H
	#include <curses.h>
#else
	#error "SysV or X/Open-compatible Curses header file required"
#endif

#include "color.h"
#include "songlist.h"
#include "command.h"
#include <sys/time.h>
#include <vector>

using namespace std;

#define WWINDOW(x)	dynamic_cast<Window *>(x)
#define WCONSOLE(x)	dynamic_cast<Wconsole *>(x)
#define WMAIN(x)	dynamic_cast<Wmain *>(x)
#define WSONGLIST(x)	dynamic_cast<Wsonglist *>(x)

typedef struct
{
	int	left;
	int	top;
	int	right;
	int	bottom;
}
Rect;

class Window
{
	protected:
		WINDOW *	window;

	public:
		Window();

		Rect		rect;
		bool		need_draw;

		/* Set dimensions and update ncurses window */
		void		set_dimensions(int top, int left, int bottom, int right);

		/* Window height */
		unsigned int	height();

		/* Draw all lines on rect */
		void		draw();

		/* Queue draw until next tick */
		void		qdraw();

		/* Flush to ncurses */
		void		flush();

		/* Clear this window */
		void		clear();
		void		clear(Color * c);
		void		clearline(int y, Color * c);

		/* Is this window visible? */
		virtual bool	visible() { return true; };

		/* Draw one line on rect */
		virtual void	drawline(int y) = 0;

		/* Draw text somewhere */
		void		print(Color * c, int y, int x, const char * fmt, ...);
};

class Wmain : public Window
{
	protected:

	public:
		/* Which context should commands be accepted in */
		int		context;

		/* Scroll position */
		unsigned int	position;

		/* Cursor position */
		unsigned int	cursor;

		/* Window title */
		string		title;


		Wmain();
		virtual unsigned int	height();

		/* Draw all lines and update readout */
		virtual void	draw();

		/* Scroll window */
		virtual void	scroll_window(int offset);

		/* Move cursor inside window */
		virtual void	move_cursor(int offset);

		/* Set absolute window/cursor position */
		virtual void	set_position(unsigned int absolute);
		virtual void	set_cursor(unsigned int absolute);

		/* List size */
		virtual unsigned int content_size() = 0;

		/* Is this window visible? */
		bool		visible();
};

class Wconsole : public Wmain
{
	public:
		Wconsole() { context = CONTEXT_CONSOLE; };

		void		drawline(int rely);
		unsigned int	content_size();
		void		move_cursor(int offset);
		void		set_cursor(unsigned int absolute);
};

class Wsonglist : public Wmain
{
	private:
		vector<unsigned int>	column_len;

	public:
		Wsonglist() { context = CONTEXT_SONGLIST; };

		void			draw();
		void			drawline(int rely);
		unsigned int		height();
		unsigned int		content_size();
		void			move_cursor(int offset);

		/* Pointer to connected songlist */
		Songlist *		songlist;

		/* Pointer to song beneath cursor */
		Song *			cursorsong();

		/* Returns all selected songs, or cursor song if none */
		selection_t		get_selection(long multiplier);

		/* Update column lengths */
		void			update_column_length();
};

class Wtopbar : public Window
{
	public:
		void		drawline(int rely);
};

class Wstatusbar : public Window
{
	public:
		Wstatusbar();

		void		drawline(int rely);

		struct timeval	cl;
		struct timeval	cl_reset;
		bool		cl_isreset;
};

class Wreadout : public Window
{
	public:
		void		drawline(int rely);
};

class Windowmanager
{
	private:
		vector<Wmain *>		windows;

		/* Active window index */
		unsigned int		active_index;
	
	public:
		Windowmanager();
		~Windowmanager();

		/* What kind of input events are accepted right now */
		bool			ready;

		/* What kind of input events are accepted right now */
		int			context;

		/* Initialize ncurses stuff */
		void			init_ncurses();

		/* Redraw all visible windows */
		void			draw();

		/* Redraw all windows flagged with qdraw */
		void			qdraw();

		/* Flush ncurses buffer */
		void			flush();

		/* Cycle window list */
		void			cycle(int offset);

		/* Activate a window */
		bool			activate(Wmain * nactive);

		/* Set left/right/top/bottom layout for all panels */
		void			detect_dimensions();

		/* Trigger the bell */
		void			bell();

		/* Activate the last used window */
		bool			toggle();

		/* Activate a window with given title, case insensitive */
		bool			go(string title);
		
		/* Activate window N */
		bool			go(unsigned int index);

		/* Update column lengths in all windows */
		void			update_column_length();

		Wconsole *		console;
		Wsonglist *		playlist;
		Wsonglist *		library;

		Wmain *			last_active;
		Wmain *			active;
		Wtopbar *		topbar;
		Wstatusbar *		statusbar;
		Wreadout *		readout;
};

#endif /* _PMS_WINDOW_H_ */
