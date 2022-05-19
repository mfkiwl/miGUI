#pragma once
#ifndef AP_PL

//#include <string>
#include "framework/String.h"

#include <vector>

class ListboxPlaylist : public mgf::ListBox
{
public:
	ListboxPlaylist(mgf::Window* w);
	virtual ~ListboxPlaylist();
	virtual wchar_t OnTextInputCharEnter(ListBox*, wchar_t) override;
	virtual int OnTextInputEndEdit(ListBox*, int i, const wchar_t* str, void* editItem) override;
	virtual int OnIsItemSelected(ListBox* e, void* item) override;
	virtual void OnItemClick(ListBox* e, void* item, uint32_t itemIndex, uint32_t mouseButton) override;
	virtual int OnDrawItem(ListBox*, void* item, uint32_t itemIndex, wchar_t** text, uint32_t* textlen) override;
};

class PlayList;
class PlayListManager;

struct PlaylistNode
{
	PlaylistNode() {}
	~PlaylistNode() {}

	mgf::StringW m_audioFilePath;
	uint32_t m_begin = 0;
	uint32_t m_end = 0;

	bool m_isPlaying = false;
	bool m_isSelected = false;

	mgf::StringW m_info_number;
	mgf::StringW m_info_title;
	mgf::StringW m_info_duration;

	PlayList* m_playlist = 0;
};

class PlayList
{
public:
	PlayList();
	~PlayList();

	void RenameFile();
	void Save();

	mgf::StringW m_name;
	mgf::StringW m_filePath;
	mgf::StringW m_fileName;

	std::vector<PlaylistNode*> m_nodes;
	PlayListManager* m_mgr = 0;

	void Free();

	bool m_isSelected = false;
};


class PlayListManager : public mgf::BaseClass
{
	mgf::Popup* m_listBoxPopup = 0;

	std::vector<PlayList*> m_playlists;

	mgf::StringW m_musicDir;
	mgf::StringW m_playlistDir; 

	mgf::StringW m_stateFilePath;

	PlayList* m_playPlaylist = 0; // active for playing
	PlayList* m_editPlaylist = 0; // active for table

	PlayList* m_playlistOnPopup = 0;

	void _updateGUIListbox();
	void _updateGUITable();
public:
	PlayListManager();
	virtual ~PlayListManager();
	
	mgf::StringW CheckPLName(mgf::StringW*);

	void AddNew();
	
	void LoadPlaylist(const wchar_t*);
	void SaveStateFile();

	void SetEditPlaylist(PlayList*);
	void SelectPlaylist(PlayList*);
	void ShowListboxPopup(PlayList*);

	void PopupOpenPlaylist();
	void PopupPlayPlaylist();
	void PopupDeletePlaylist();
	void PopupShowPlaylistInExplorer();
	
	void DeletePlaylist(PlayList*);

	void AddTrackToEditPlaylist(const wchar_t*);
	bool IsSupportedFormat(const wchar_t*);
	void SaveEditPlaylist();
	void StopPlaying();
	void PlayTrack(PlaylistNode* nd);
};

#endif // !AP_PL
