/* the Music Player Daemon (MPD)
 * (c)2003-2004 by Warren Dukes (shank@mercury.chem.pitt.edu)
 * This project's homepage is: http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <sys/param.h>

#define PLAYER_STATE_STOP	0
#define PLAYER_STATE_PAUSE	1
#define PLAYER_STATE_PLAY	2

#define PLAYER_ERROR_NOERROR		0
#define PLAYER_ERROR_FILE		1
#define PLAYER_ERROR_AUDIO		2
#define PLAYER_ERROR_SYSTEM		3
#define PLAYER_ERROR_UNKTYPE		4
#define PLAYER_ERROR_FILENOTFOUND	5

/* 0->1->2->3->5 regular playback
 *        ->4->0 don't play queued song
 */
#define PLAYER_QUEUE_BLANK	0
#define PLAYER_QUEUE_FULL	1
#define PLAYER_QUEUE_DECODE	2
#define PLAYER_QUEUE_PLAY	3
#define PLAYER_QUEUE_STOP	4
#define PLAYER_QUEUE_EMPTY	5

#define PLAYER_QUEUE_UNLOCKED	0
#define PLAYER_QUEUE_LOCKED	1

typedef struct _PlayerControl {
	int decodeType;
	int stop;
	int play;
	int pause;
	int state;
	int closeAudio;
	int error;
	unsigned long bitRate;
	float beginTime;
	float totalTime;
	float elapsedTime;
	char file[MAXPATHLEN+1];
	char erroredFile[MAXPATHLEN+1];
	int queueState;
	int queueLockState;
	int lockQueue;
	int unlockQueue;
	int seek;
	double seekWhere;
	float crossFade;
	int softwareVolume;
	double totalPlayTime;
} PlayerControl;

void player_sigHandler(int signal);

int playerPlay(FILE * fp, char * utf8file);

int playerPause(FILE * fp);

int playerStop(FILE * fp);

void playerCloseAudio();

void playerKill();

void playerProcessMessages();

int getPlayerTotalTime();

int getPlayerElapsedTime();

unsigned long getPlayerBitRate();

int getPlayerState();

void clearPlayerError();

char * getPlayerErrorStr();

int getPlayerError();

int playerInit();

int queueSong(char * utf8file);

int getPlayerQueueState();

void setQueueState(int queueState);

void playerQueueLock();

void playerQueueUnlock();

int playerSeek(FILE * fp, char * file, float time);

void setPlayerCrossFade(float crossFadeInSeconds);

float getPlayerCrossFade();

int getPlayerSoftwareVolume();

void setPlayerSoftwareVolume(int volume);

double getPlayerTotalPlayTime();

#endif
