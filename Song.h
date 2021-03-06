#ifndef SONG_H
#define SONG_H





#include <memory>
#include <vector>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QCoreApplication>
#include <QMutex>
#include "DatedOptional.h"





// fwd:
class QSqlQuery;
class Song;
using SongPtr = std::shared_ptr<Song>;





/** Binds an on-disk file with its metadata stored in our DB.
Each disk file has one Song instance; multiple instances may have the same audio data (matched using Hash), these all
have their SharedData pointing to the same object. This way the data that is common for the audio is stored shared,
while data pertaining to the file itself is stored separately for duplicates. */
class Song:
	public std::enable_shared_from_this<Song>
{
	Q_DECLARE_TR_FUNCTIONS(Song)

public:
	/** A simple structure for a set of tags, extracted from a single source.
	Since there are three tag sources with common metadata (ID3, FileName, Manual), this stores
	the metadata for each source. */
	struct Tag
	{
		DatedOptional<QString> m_Title;
		DatedOptional<QString> m_Author;
		DatedOptional<QString> m_Genre;
		DatedOptional<double>  m_MeasuresPerMinute;
	};


	/** Rating in various categories (#100) */
	struct Rating
	{
		DatedOptional<double> m_RhythmClarity;
		DatedOptional<double> m_GenreTypicality;
		DatedOptional<double> m_Popularity;
		DatedOptional<double> m_Local;
	};


	/** Data that can be shared among multiple files that represent the same song (same hash). */
	struct SharedData
	{
		QByteArray m_Hash;
		QVariant m_Length;
		QVariant m_LastPlayed;
		Rating m_Rating;
		Tag m_TagManual;
		mutable QMutex m_Mtx;  // Mutex protecting m_Duplicates against multithreaded access
		std::vector<Song *> m_Duplicates;  // All songs having the same hash
		DatedOptional<double> m_SkipStart;  // Where to start playing
		DatedOptional<QString> m_Notes;

		SharedData(
			const QByteArray & a_Hash,
			QVariant && a_Length,
			QVariant && a_LastPlayed,
			Rating && a_Rating,
			Tag && a_TagManual,
			DatedOptional<double> && a_SkipStart,
			DatedOptional<QString> && a_Notes
		):
			m_Hash(a_Hash),
			m_Length(std::move(a_Length)),
			m_LastPlayed(std::move(a_LastPlayed)),
			m_Rating(std::move(a_Rating)),
			m_TagManual(std::move(a_TagManual)),
			m_SkipStart(std::move(a_SkipStart)),
			m_Notes(std::move(a_Notes))
		{
		}

		SharedData(const QByteArray & a_Hash):
			m_Hash(a_Hash)
		{
		}

		void addDuplicate(Song * a_Duplicate);
		void delDuplicate(const Song * a_Duplicate);
		size_t duplicatesCount() const;
		std::vector<Song *> duplicates() const;
	};

	using SharedDataPtr = std::shared_ptr<SharedData>;


	/** Creates a new instance that has only the file name and size set, all the other metadata are empty.
	Used when adding new files. */
	explicit Song(
		const QString & a_FileName,
		SharedDataPtr a_SharedData
	);

	/** Creates a new instance with all fields set.
	Used when loading songs from the DB. */
	explicit Song(
		QString && a_FileName,
		SharedDataPtr a_SharedData,
		Tag && a_TagFileName,
		Tag && a_TagId3,
		QVariant && a_LastTagRescanned,
		QVariant && a_NumTagRescanAttempts
	);

	~Song();

	const QString & fileName() const { return m_FileName; }
	const QByteArray & hash() const { return m_SharedData->m_Hash; }
	const Tag & tagManual() const { return m_SharedData->m_TagManual; }
	const Tag & tagFileName() const { return m_TagFileName; }
	const Tag & tagId3() const { return m_TagId3; }
	const QVariant & lastTagRescanned() const { return m_LastTagRescanned; }
	const QVariant & numTagRescanAttempts() const { return m_NumTagRescanAttempts; }
	const SharedDataPtr & sharedData() const { return m_SharedData; }
	const DatedOptional<QString> & notes() const { return m_SharedData->m_Notes; }

	// These return the value from the first tag which has the value valid, in the order or Manual, Id3, FileName
	const DatedOptional<QString> & primaryAuthor() const;
	const DatedOptional<QString> & primaryTitle() const;
	const DatedOptional<QString> & primaryGenre() const;
	const DatedOptional<double>  & primaryMeasuresPerMinute() const;

	// These return values from the shared data, if available:
	const QVariant & length()     const { return m_SharedData->m_Length; }
	const QVariant & lastPlayed() const { return m_SharedData->m_LastPlayed; }
	const Rating &   rating()     const { return m_SharedData->m_Rating; }
	const DatedOptional<double> skipStart() const;

	/** Returns whether the disk file still exists and it matches our stored hash. */
	bool isStillValid() const;

	/** Updates the length of the song, in seconds. */
	void setLength(double a_Length);

	// Setters that redirect into the Manual tag:
	void setAuthor(QVariant a_Author) { m_SharedData->m_TagManual.m_Author = a_Author; }
	void setTitle(QVariant a_Title) { m_SharedData->m_TagManual.m_Title = a_Title; }
	void setGenre(const QString & a_Genre) { m_SharedData->m_TagManual.m_Genre = a_Genre; }
	void setMeasuresPerMinute(double a_MeasuresPerMinute) { m_SharedData->m_TagManual.m_MeasuresPerMinute = a_MeasuresPerMinute; }

	// Setters for specific tags:
	void setManualAuthor(QVariant a_Author) { m_SharedData->m_TagManual.m_Author = a_Author; }
	void setManualTitle(QVariant a_Title) { m_SharedData->m_TagManual.m_Title = a_Title; }
	void setManualGenre(const QString & a_Genre) { m_SharedData->m_TagManual.m_Genre = a_Genre; }
	void setManualMeasuresPerMinute(double a_MeasuresPerMinute) { m_SharedData->m_TagManual.m_MeasuresPerMinute = a_MeasuresPerMinute; }
	void setManualTag(const Tag & a_Tag) { m_SharedData->m_TagManual = a_Tag; }
	void setId3Author(const QString & a_Author) { m_TagId3.m_Author = a_Author; }
	void setId3Title(const QString & a_Title)   { m_TagId3.m_Title  = a_Title; }
	void setId3Genre(const QString & a_Genre)   { m_TagId3.m_Genre  = a_Genre; }
	void setId3MeasuresPerMinute(double a_MPM)  { m_TagId3.m_MeasuresPerMinute = a_MPM; }
	void setId3Tag(const Tag & a_Tag) { m_TagId3 = a_Tag; }
	void setFileNameAuthor(const QString & a_Author) { m_TagFileName.m_Author = a_Author; }
	void setFileNameTitle(const QString & a_Title)   { m_TagFileName.m_Title  = a_Title; }
	void setFileNameGenre(const QString & a_Genre)   { m_TagFileName.m_Genre  = a_Genre; }
	void setFileNameMeasuresPerMinute(double a_MPM)  { m_TagFileName.m_MeasuresPerMinute = a_MPM; }
	void setFileNameTag(const Tag & a_Tag) { m_TagFileName = a_Tag; }

	// Basic setters:
	void setLastTagRescanned(const QDateTime & a_LastTagRescanned) { m_LastTagRescanned = a_LastTagRescanned; }
	void setNumTagRescanAttempts(int a_NumTagRescanAttempts) { m_NumTagRescanAttempts = a_NumTagRescanAttempts; }
	void setNotes(const QString & a_Notes) { m_SharedData->m_Notes = a_Notes; }

	// Setters that preserve the date information:
	void setNotes(const DatedOptional<QString> & a_Notes) { m_SharedData->m_Notes = a_Notes; }

	// Setters that move-preserve the date information:
	void setNotes(DatedOptional<QString> && a_Notes) { m_SharedData->m_Notes = std::move(a_Notes); }

	/** Sets the local rating, if SharedData is present; otherwise ignored. */
	void setLocalRating(double a_Value);

	/** Sets the skip start, if SharedData is present; otherwise ignored. */
	void setSkipStart(double a_Seconds);

	/** Removes the skip-start, if SharedData is present; otherwise ignored. */
	void delSkipStart();

	/** Returns true if a tag rescan is needed for the song
	(the tags are empty and the scan hasn't been performed already). */
	bool needsTagRescan() const;

	/** Returns all the warnings that this song has, each as a separate string.
	Returns an empty string list if the song has no warnings. */
	QStringList getWarnings() const;

	/** Returns the first of the three variants that is non-empty.
	Returns the third if all are null.*/
	template <typename T> static const DatedOptional<T> & primaryValue(
		const DatedOptional<T> & a_First,
		const DatedOptional<T> & a_Second,
		const DatedOptional<T> & a_Third
	)
	{
		if (!a_First.isEmpty())
		{
			return a_First;
		}
		if (!a_Second.isEmpty())
		{
			return a_Second;
		}
		return a_Third;
	}

	/** Returns the competition tempo range for the specified genre.
	If the genre is not known, returns 0 .. MAX_USHORT */
	static std::pair<double, double> competitionTempoRangeForGenre(const QString & a_Genre);

	/** Returns all the songs that have the same hash as this song, including this. */
	std::vector<Song *> duplicates();

	/** Returns all recognized genres, in a format suitable for QComboBox items. */
	static QStringList recognizedGenres();

	/** Returns the best guess of the MPM for a song of the specified genre from the specified tempo.
	Assumes that the tempo is a reasonable multiple of the destination MPM; genre is used for triplet tempos. */
	static double foldTempoToMPM(double a_Tempo, const DatedOptional<QString> & a_Genre);


protected:

	QString m_FileName;
	SharedDataPtr m_SharedData;
	Tag m_TagFileName;
	Tag m_TagId3;
	QVariant m_LastTagRescanned;
	QVariant m_NumTagRescanAttempts;

	/** An empty variant returned when there's no shared data for a song */
	static QVariant m_Empty;

	/** An empty rating returned when there's no shared ddata for a song. */
	static Rating m_EmptyRating;
};

Q_DECLARE_METATYPE(SongPtr);
Q_DECLARE_METATYPE(Song::SharedDataPtr);





#endif // SONG_H
