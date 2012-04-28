#ifndef QSVSYNTAXHIGHLIGHTERBASE_H
#define QSVSYNTAXHIGHLIGHTERBASE_H

#include "qate/qateblockdata.h"

class QsvSyntaxHighlighterBase
{
public:
	QsvSyntaxHighlighterBase();
	virtual ~QsvSyntaxHighlighterBase();
	
	virtual void toggleBookmark(QTextBlock &block) = 0;
	virtual void removeModification(QTextBlock &block) = 0;
	virtual void setBlockModified(QTextBlock &block, bool on) = 0;
	virtual bool isBlockModified(QTextBlock &block) = 0;
	virtual bool isBlockBookmarked(QTextBlock &block) = 0;
	virtual Qate::BlockData::LineFlags getBlockFlags(QTextBlock &block) = 0;

	void setMatchBracketList( const QString &m );
	const QString getMatchBracketList();
	void highlightBlock(const QString &text);
	void setTextDocument(QTextDocument * document);

protected:
	QString m_matchBracketsList;
};

#endif // QSVSYNTAXHIGHLIGHTERBASE_H
