#ifndef LENGTHHASHCALCULATOR_H
#define LENGTHHASHCALCULATOR_H





#include <atomic>
#include <QObject>
#include "Song.h"





/** Calculates the length and hash of a song file.
The hash used is a SHA1 checksum of the raw audio data in the file's audio track. This is considered immune
against changes in the ID3 tag.
The actual hashing takes place in a background task. */
class LengthHashCalculator:
	public QObject
{
	using Super = QObject;

	Q_OBJECT


public:

	LengthHashCalculator();

	/** Returns the number of songs that are queued for hashing. */
	int queueLength() { return m_QueueLength.load(); }


protected:

	/** The number of songs that are queued for hashing. */
	std::atomic<int> m_QueueLength;


public slots:

	/** Queues the specified file for length and hash in a background task.
	After the hash has been calculated, either fileHashCalculated() or fileHashFailed() is emitted. */
	void queueHashFile(const QString & a_FileName);

	/** Queues the specified song for length calculation in a background task.
	Tries the first "duplicate" that actually exists in the filesystem.
	After the length has been calculated, either fileLengthCalculated() or fileLengthFailed() is emitted. */
	void queueLengthSong(Song::SharedDataPtr a_SharedData);


signals:

	/** Emitted after successfully lengthing and hashing the song file. */
	void fileHashCalculated(const QString & a_FileName, const QByteArray & a_Hash, double a_LengthSec);

	/** Emitted after encountering a problem while lengthing or hashing a file. */
	void fileHashFailed(const QString & a_File);

	/** Emitted after successfully lengthing the song file. */
	void songLengthCalculated(Song::SharedDataPtr a_SharedData, double a_LengthSec);

	/** Emitted after encountering a problem while lengthing a song. */
	void songLengthFailed(Song::SharedDataPtr a_SharedData);
};





#endif // LENGTHHASHCALCULATOR_H
