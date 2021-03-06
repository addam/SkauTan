#ifndef DLGTEMPODETECT_H
#define DLGTEMPODETECT_H





#include <memory>
#include <vector>
#include <QDialog>
#include "Song.h"
#include "TempoDetector.h"





// fwd:
class QLabel;
class Database;
namespace Ui
{
	class DlgTempoDetect;
}





class DlgTempoDetect:
	public QDialog
{
	using Super = QDialog;

	Q_OBJECT


public:

	explicit DlgTempoDetect(Database & a_DB, SongPtr a_Song, QWidget * a_Parent = nullptr);

	~DlgTempoDetect();


private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::DlgTempoDetect> m_UI;

	/** The DB to which the song belongs. */
	Database & m_DB;

	/** The song being analyzed. */
	SongPtr m_Song;

	/** The history of detections on this song. */
	std::vector<std::shared_ptr<TempoDetector::Result>> m_History;

	/** Set to true while the code updates the UI, to avoid recalculations.
	If false, the UI changes come from the user. */
	bool m_IsInternalChange;

	/** The current delay, as used in delayedDetectTempo's timer.
	Updated every 50 msec, when it reaches 1, the tempo detection is started; doesn't go below 0. */
	int m_TempoDetectDelay;

	/** The tempo detector instance on which to run the detections. */
	std::unique_ptr<TempoDetector> m_Detector;

	/** The "Detection in progress" label shown over the result list. */
	std::unique_ptr<QLabel> m_ProgressLabel;


	/** Fills in the values for options into the UI selectors. */
	void initOptionsUi();

	/** Updates the UI to match the specified options. */
	void selectOptions(const TempoDetector::Options & a_Options);

	/** Reads the UI settings and returns an Options object initialized by those settings. */
	TempoDetector::Options readOptionsFromUi();

	/** Fills the result into the UI and stores it in History (updates if present). */
	void fillInResults(const TempoDetector::Result & a_Results);

	/** Updates the specified row in the History view. */
	void updateHistoryRow(int a_Row);


private slots:

	/** Starts a timer, upon timeout reads the UI values and starts detecting the tempo.
	Calling this again before the timer elapses restarts the timer from the beginning.
	Used for user-editable values that may require several calls before the entire value is input. */
	void delayedDetectTempo();

	/** Reads the UI values and starts detecting the tempo.
	If the options set in the UI are already in m_History, uses the results from there instead. */
	void detectTempo();

	/** Emitted by TempoDetector after it scans the song.
	Adds the result to m_History and updates the UI. */
	void songScanned(SongPtr a_Song, TempoDetector::ResultPtr a_Result);

	/** Asks the user for a filename, then saves the debug audio data with beats to the file. */
	void saveDebugBeats();

	/** Asks the user for a filename, then saves the debug audio data with levels to the file. */
	void saveDebugLevels();
};





#endif // DLGTEMPODETECT_H
