/* vi:set ts=8 sts=8 sw=8 noet:
 *
 * PMS  <<Practical Music Search>>
 * Copyright (C) 2006-2015  Kim Tore Jensen
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
 *
 * types.h
 * Structs used throughout the program
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <assert.h>
#include "color.h"

typedef signed long song_t;

/*
 * Pending actions
 */
typedef enum
{
	PEND_NONE,


	PEND_EXEC,
	PEND_VERSION,
	PEND_RESIZE, //FIXME
	PEND_REDRAW,
	PEND_REHASH,
	PEND_SOURCE,
	PEND_QUIT,
	PEND_SHELL,
	PEND_SHOW_INFO,

	PEND_PASSWORD,
	PEND_UPDATE_DB,
	PEND_VOLUME,
	PEND_MUTE,
	PEND_CROSSFADE,
	PEND_REPEAT,
	PEND_RANDOM,
	PEND_SINGLE,
	PEND_CONSUME,

	PEND_DELETE,
	PEND_SEEK,
	PEND_COMMANDMODE,
	PEND_SELECT,
	PEND_UNSELECT,
	PEND_TOGGLESELECT,
	PEND_CLEARSELECTION,
	PEND_SELECTALL,

	PEND_RETURN,
	PEND_RETURN_ESCAPE,

	PEND_JUMPMODE,
	PEND_FILTERMODE,
	PEND_CLEARFILTERS,
	PEND_PREVOF,
	PEND_NEXTOF,
	PEND_GOTORANDOM,
	PEND_TEXT_UPDATED,
	PEND_TEXT_RETURN,
	PEND_TEXT_ESCAPE,

	PEND_PLAY,
	PEND_PLAYARTIST,
	PEND_PLAYALBUM,
	PEND_PLAYRANDOM,
	PEND_ADD,
	PEND_ADDTO,
	PEND_ADDARTIST,
	PEND_ADDALBUM,
	PEND_ADDRANDOM,
	PEND_ADDALL,
	PEND_MOVEITEMS,
	PEND_NEXT,
	PEND_REALLY_NEXT,
	PEND_PREV,
	PEND_PAUSE,
	PEND_TOGGLEPLAY,
	PEND_STOP,
	PEND_SHUFFLE,
	PEND_CLEAR,
	PEND_CROP,
	PEND_CROPSELECTION,

	PEND_CREATEPLAYLIST,
	PEND_SAVEPLAYLIST,
	PEND_DELETEPLAYLIST,
	PEND_NEXTWIN,
	PEND_PREVWIN,
	PEND_CHANGEWIN,
	PEND_LASTWIN,
	PEND_SHOWBIND,
	PEND_ACTIVATELIST,
	PEND_JUMPNEXT,
	PEND_JUMPPREV,
	PEND_GOTO_CURRENT,
	PEND_MOVE_UP,
	PEND_MOVE_DOWN,
	PEND_MOVE_HALFPGUP,
	PEND_MOVE_HALFPGDN,
	PEND_MOVE_PGUP,
	PEND_MOVE_PGDN,
	PEND_MOVE_HOME,
	PEND_MOVE_END,
	PEND_SCROLL_UP,
	PEND_SCROLL_DOWN,
	PEND_CENTER_CURSOR

}
pms_pending_keys;


/*
 * Scroll mode
 */
typedef enum
{
	SCROLL_NORMAL = 0,
	SCROLL_CENTERED

}
pms_scroll_mode;

/*
 * Statusbar modes
 */
typedef enum
{
	STOK = 0,
	STERR = 1

}
statusbar_mode;

/*
 * Crop types
 */
enum
{
	CROP_PLAYING = 0,
	CROP_SELECTION
};

/*
 * Selection modes
 */
enum
{
	SELECT_OFF = 0,
	SELECT_ON,
	SELECT_TOGGLE,
	SELECT_ALL,
	SELECT_CLEAR
};


#endif /* _TYPES_H_ */
