#include "SongDecoder.h"
#include <QtConcurrent/QtConcurrent>





////////////////////////////////////////////////////////////////////////////////
// SongDecoder:

SongDecoder::SongDecoder(const QAudioFormat & a_OutputFormat, SongPtr a_Song):
	Super(a_OutputFormat),
	m_Song(a_Song),
	m_Termination(0)
{
	QtConcurrent::run(this, &SongDecoder::decode);
	if (a_Song->skipStart().isPresent())
	{
		qDebug() << "Skip-start active: " << a_Song->skipStart().value();
		seekToFrame(a_OutputFormat.framesForDuration(static_cast<qint64>(a_Song->skipStart().value() * 1000000)));
	}
}





SongDecoder::~SongDecoder()
{
	abort();
	m_Termination.acquire();
}





void SongDecoder::decode()
{
	decodeInternal();
	m_Termination.release();
}





void SongDecoder::decodeInternal()
{
	// Open the file:
	m_FmtCtx = AVPP::Format::createContext(m_Song->fileName());
	if (m_FmtCtx == nullptr)
	{
		qDebug() << "Decoding failed for file " << m_Song->fileName();
		abortWithError();
		return;
	}
	m_FmtCtx->routeAudioTo(this);
	m_FmtCtx->decode();
	qDebug() << "Decoding has finished file " << m_Song->fileName();
	decodedEOF();
}
