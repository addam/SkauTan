#include "PlaylistItemSong.h"
#include <QAudioFormat>
#include "SongDecoder.h"





PlaylistItemSong::PlaylistItemSong(SongPtr a_Song, Template::ItemPtr a_TemplateItem):
	m_Song(a_Song),
	m_TemplateItem(a_TemplateItem),
	m_DurationLimit(
		((a_TemplateItem != nullptr) && a_TemplateItem->durationLimit().isPresent()) ?
		a_TemplateItem->durationLimit().value() : -1
	)
{
}





QString PlaylistItemSong::displayName() const
{
	// TODO: Decide whether to show full filename, or just partial
	return m_Song->fileName();
}





QString PlaylistItemSong::displayAuthor() const
{
	return m_Song->primaryAuthor().valueOrDefault();
}





QString PlaylistItemSong::displayTitle() const
{
	return m_Song->primaryTitle().valueOrDefault();
}





double PlaylistItemSong::displayLength() const
{
	if (m_Song->length().isValid())
	{
		return m_Song->length().toDouble();
	}
	return -1;
}





QString PlaylistItemSong::displayGenre() const
{
	if (!m_Song->primaryGenre().isEmpty())
	{
		return m_Song->primaryGenre().value();
	}
	return QString();
}





double PlaylistItemSong::displayTempo() const
{
	if (m_Song->primaryMeasuresPerMinute().isPresent())
	{
		return m_Song->primaryMeasuresPerMinute().value();
	}
	return -1;
}





void PlaylistItemSong::setDurationLimit(double a_Seconds)
{
	m_DurationLimit = a_Seconds;
}





PlaybackBuffer * PlaylistItemSong::startDecoding(const QAudioFormat & a_Format)
{
	Q_UNUSED(a_Format);

	return new SongDecoder(a_Format, m_Song);
}
