#ifndef PLAYLIST_H
#define PLAYLIST_H





#include <memory>
#include <vector>
#include <QObject>
#include "IPlaylistItem.h"
#include "Template.h"





// fwd:
class Database;
class Song;
using SongPtr = std::shared_ptr<Song>;





/** Container for a single playlist.
Stores the items to be played, the currently played item, timings etc. */
class Playlist:
	public QObject
{
	using Super = QObject;
	Q_OBJECT

public:
	Playlist();

	const std::vector<IPlaylistItemPtr> & items() const { return m_Items; }

	/** Adds the specified item to the bottom of the playlist. */
	void addItem(IPlaylistItemPtr a_Item);

	/** Moves the item at a_FromIdex to a_ToIdx, shifting the items in between accordingly.
	Doesn't emit any signal. */
	void moveItem(size_t a_FromIdx, size_t a_ToIdx);

	/** Deletes the item at the specified index.
	Emits the appropriate signals. */
	void deleteItem(int a_Index);

	/** Reletes the items at the specified indices.
	Emits the appropriate signals. */
	void deleteItems(std::vector<int> && a_Indices);

	/** Returns the current item being played, or to be played when the playback is started. */
	IPlaylistItemPtr currentItem() const;

	/** Returns the current item's index in the playlist. */
	int currentItemIndex() const { return m_CurrentItemIdx; }

	/** Moves the current item to the next in the list.
	Returns true if the current item changed, false if not (end of list / no items). */
	bool nextItem();

	/** Moves the current item to the previous in the list.
	Returns true if the current item changed, false if not (start of list / no items). */
	bool prevItem();

	/** Sets the current item to the specified index.
	Ignored if the index is invalid.
	Returns true iff the current item has been set. */
	bool setCurrentItem(int a_Index);

	/** Sets the current item to the specified item.
	Ignored if the specified item is not in the playlist.
	Returns true iff the current item has been set. */
	bool setCurrentItem(const IPlaylistItem * a_Item);

	/** Adds new random songs based on the specified template, using songs from the specified DB. */
	void addFromTemplate(const Database & a_DB, const Template & a_Template);

	/** Adds a new random song based on the specified template item, using songs from the specified DB. */
	bool addFromTemplateItem(const Database & a_DB, const Template::Item & a_Item);


protected:

	/** All the items on the playlist. */
	std::vector<IPlaylistItemPtr> m_Items;

	/** The item in m_Items currently being played, or to be played when the playback is started. */
	int m_CurrentItemIdx;

	/** If true, m_CurrentItem is being played back. */
	bool m_IsPlaying;


	/** Returns the relative "weight" of a song, when choosing by a template.
	The weight is adjusted based on song's rating, last played date, if it is already in the playlist etc. */
	int getSongWeight(const Database & a_DB, const Song & a_Song);

signals:

	/** Emitted after the specified item is added to the bottom of the playlist. */
	void itemAdded(IPlaylistItem * a_Item);

	/** Emitted before the specified item is deleted from the playlist. */
	void itemDeleting(IPlaylistItem * a_Item, int a_Index);

	/** Emitted after the index for the current item is changed. */
	void currentItemChanged(int a_CurrentItemIdx);


public slots:

	/** Removes any playlist entries relevant to the specified song. */
	void removeSong(SongPtr a_Song);
};

using PlaylistPtr = std::shared_ptr<Playlist>;





#endif // PLAYLIST_H
