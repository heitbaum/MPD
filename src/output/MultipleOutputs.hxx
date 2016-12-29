/*
 * Copyright 2003-2016 The Music Player Daemon Project
 * http://www.musicpd.org
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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Functions for dealing with all configured (enabled) audion outputs
 * at once.
 *
 */

#ifndef OUTPUT_ALL_H
#define OUTPUT_ALL_H

#include "AudioFormat.hxx"
#include "ReplayGainMode.hxx"
#include "Chrono.hxx"
#include "Compiler.h"

#include <vector>

#include <assert.h>

class MusicBuffer;
class MusicPipe;
class EventLoop;
class MixerListener;
class AudioOutputClient;
struct MusicChunk;
struct AudioOutput;
struct ReplayGainConfig;

class MultipleOutputs {
	MixerListener &mixer_listener;

	std::vector<AudioOutput *> outputs;

	AudioFormat input_audio_format = AudioFormat::Undefined();

	/**
	 * The #MusicBuffer object where consumed chunks are returned.
	 */
	MusicBuffer *buffer = nullptr;

	/**
	 * The #MusicPipe object which feeds all audio outputs.  It is
	 * filled by audio_output_all_play().
	 */
	MusicPipe *pipe = nullptr;

	/**
	 * The "elapsed_time" stamp of the most recently finished
	 * chunk.
	 */
	SignedSongTime elapsed_time = SignedSongTime::Negative();

public:
	/**
	 * Load audio outputs from the configuration file and
	 * initialize them.
	 */
	MultipleOutputs(MixerListener &_mixer_listener);
	~MultipleOutputs();

	void Configure(EventLoop &event_loop,
		       const ReplayGainConfig &replay_gain_config,
		       AudioOutputClient &client);

	/**
	 * Returns the total number of audio output devices, including
	 * those which are disabled right now.
	 */
	gcc_pure
	unsigned Size() const {
		return outputs.size();
	}

	/**
	 * Returns the "i"th audio output device.
	 */
	const AudioOutput &Get(unsigned i) const {
		assert(i < Size());

		return *outputs[i];
	}

	AudioOutput &Get(unsigned i) {
		assert(i < Size());

		return *outputs[i];
	}

	/**
	 * Returns the audio output device with the specified name.
	 * Returns nullptr if the name does not exist.
	 */
	gcc_pure
	AudioOutput *FindByName(const char *name) const;

	/**
	 * Checks the "enabled" flag of all audio outputs, and if one has
	 * changed, commit the change.
	 */
	void EnableDisable();

	/**
	 * Opens all audio outputs which are not disabled.
	 *
	 * Throws #std::runtime_error on error.
	 *
	 * @param audio_format the preferred audio format
	 * @param _buffer the #music_buffer where consumed #MusicChunk objects
	 * should be returned
	 */
	void Open(const AudioFormat audio_format, MusicBuffer &_buffer);

	/**
	 * Closes all audio outputs.
	 */
	void Close();

	/**
	 * Closes all audio outputs.  Outputs with the "always_on"
	 * flag are put into pause mode.
	 */
	void Release();

	void SetReplayGainMode(ReplayGainMode mode);

	/**
	 * Enqueue a #MusicChunk object for playing, i.e. pushes it to a
	 * #MusicPipe.
	 *
	 * Throws #std::runtime_error on error (all closed then).
	 *
	 * @param chunk the #MusicChunk object to be played
	 */
	void Play(MusicChunk *chunk);

	/**
	 * Checks if the output devices have drained their music pipe, and
	 * returns the consumed music chunks to the #music_buffer.
	 *
	 * @return the number of chunks to play left in the #MusicPipe
	 */
	unsigned Check();

	/**
	 * Puts all audio outputs into pause mode.  Most implementations will
	 * simply close it then.
	 */
	void Pause();

	/**
	 * Drain all audio outputs.
	 */
	void Drain();

	/**
	 * Try to cancel data which may still be in the device's buffers.
	 */
	void Cancel();

	/**
	 * Indicate that a new song will begin now.
	 */
	void SongBorder();

	/**
	 * Returns the "elapsed_time" stamp of the most recently finished
	 * chunk.  A negative value is returned when no chunk has been
	 * finished yet.
	 */
	gcc_pure
	SignedSongTime GetElapsedTime() const {
		return elapsed_time;
	}

	/**
	 * Returns the average volume of all available mixers (range
	 * 0..100).  Returns -1 if no mixer can be queried.
	 */
	gcc_pure
	int GetVolume() const;

	/**
	 * Sets the volume on all available mixers.
	 *
	 * @param volume the volume (range 0..100)
	 * @return true on success, false on failure
	 */
	bool SetVolume(unsigned volume);

	/**
	 * Similar to GetVolume(), but gets the volume only for
	 * software mixers.  See #software_mixer_plugin.  This
	 * function fails if no software mixer is configured.
	 */
	gcc_pure
	int GetSoftwareVolume() const;

	/**
	 * Similar to SetVolume(), but sets the volume only for
	 * software mixers.  See #software_mixer_plugin.  This
	 * function cannot fail, because the underlying software
	 * mixers cannot fail either.
	 */
	void SetSoftwareVolume(unsigned volume);

private:
	/**
	 * Determine if all (active) outputs have finished the current
	 * command.
	 */
	gcc_pure
	bool AllFinished() const;

	void WaitAll();

	/**
	 * Signals all audio outputs which are open.
	 */
	void AllowPlay();

	/**
	 * Opens all output devices which are enabled, but closed.
	 *
	 * @return true if there is at least open output device which
	 * is open
	 */
	bool Update(bool force);

	/**
	 * Has this chunk been consumed by all audio outputs?
	 */
	bool IsChunkConsumed(const MusicChunk *chunk) const;

	/**
	 * There's only one chunk left in the pipe (#pipe), and all
	 * audio outputs have consumed it already.  Clear the
	 * reference.
	 */
	void ClearTailChunk(const MusicChunk *chunk, bool *locked);
};

#endif
