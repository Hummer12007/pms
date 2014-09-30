/* vi:set ts=4 sts=4 sw=4 et:
 *
 * Practical Music Search
 * Copyright (c) 2006-2014 Kim Tore Jensen
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
 */

#include <mpd/client.h>

struct songlist_t;

struct mpd_connection * pms_mpd_connect(const char * server, unsigned int port, unsigned int timeout);
void pms_mpd_get_status(struct mpd_connection * connection);
enum mpd_error pms_mpd_get_playlist(struct mpd_connection * connection, struct songlist_t * songlist);
void pms_handle_mpd_idle_update(struct mpd_connection * connection, enum mpd_idle flags);
