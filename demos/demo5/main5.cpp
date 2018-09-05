#include <QApplication>
#include <QAction>
#include <QToolBar>
#include <QTimer>
#include <QMainWindow>
#include <QFileDialog>
#include <QSyntaxHighlighter>
#include <QToolButton>
#include <QPushButton>
#include <QDebug>
#include <QCommonStyle>

#include "qsvsh/qsvcolordef.h"
#include "qsvsh/qsvcolordeffactory.h"
#include "qsvsh/qsvlangdef.h"
#include "qsvsh/qsvlangdeffactory.h"
#include "qsvsh/qsvsyntaxhighlighter.h"

#include "qate/qateblockdata.h"
#include "qsvte/qsvtextedit.h"
#include "qsvte/qsvtextoperationswidget.h"
#include "qsvte/qsvsyntaxhighlighterbase.h"

class MyHighlighter: public QsvSyntaxHighlighter, public QsvSyntaxHighlighterBase {
public:
	MyHighlighter(QTextDocument *parent): QsvSyntaxHighlighter(parent) {
		setMatchBracketList("{}()[]''\"\"");
	}

	virtual void highlightBlock(const QString &text) override {
		QsvSyntaxHighlighterBase::highlightBlock(text);
		QsvSyntaxHighlighter::highlightBlock(text);
	}
	
	virtual void toggleBookmark(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return;
		data->toggleBookmark();
	}

	virtual void removeModification(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return;
		data->m_isModified = false;
	}

	virtual void setBlockModified(QTextBlock &block, bool on) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return;
		data->m_isModified =  on;
	}

	virtual bool isBlockModified(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return false;
		return data->m_isModified;
	}

	virtual bool isBlockBookmarked(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return 0;
		return data->isBookmark();
	}

	virtual Qate::BlockData::LineFlags getBlockFlags(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return 0;
		return data->m_flags;
	}
	
	virtual void clearMatchData(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return;
	}

	virtual void addMatchData(QTextBlock &block, Qate::MatchData m) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return;
		data->matches << m;
	}

	virtual QList<Qate::MatchData> getMatches(QTextBlock &block) override {
		Qate::BlockData *data = getBlockData(block);
		if (data == nullptr)
			return QList<Qate::MatchData>();
		return data->matches;
	}
	virtual QTextBlock getCurrentBlockProxy() override {
		return currentBlock();
	}

	Qate::BlockData *getBlockData(QTextBlock &block) {
		QTextBlockUserData *userData  = block.userData();
		Qate::BlockData    *blockData = nullptr;
		
		if (userData == nullptr){
//			blockData =  new Qate::BlockData();
//			block.setUserData(blockData);
		} else {
			blockData = dynamic_cast<Qate::BlockData*>(userData);
		}
		return blockData;
	}
};

class MainWindow : public QMainWindow
{
	Q_OBJECT
	QsvTextEdit *editor;
	QsvSyntaxHighlighterBase * syntax;
	QsvColorDefFactory 	 * defColors;
	QsvLangDef 		 * langDefinition;
	MyHighlighter    	 * highlight;
	QsvTextOperationsWidget  * textOpetations;
public:
	MainWindow( const QString &file )
	{
		QString dataPath  = QDir::currentPath();
		//QsvLangDefFactory::getInstanse()->addMimeTypes( "data/mime.types" );
		QsvLangDefFactory::getInstanse()->loadDirectory( "data/langs/" );
		editor           = new QsvTextEdit(this, nullptr);
		defColors        = new QsvColorDefFactory( "data/colors/kate.xml" );
		langDefinition   = QsvLangDefFactory::getInstanse()->getHighlight("1.cpp");
		highlight        = new MyHighlighter(editor->document());
		textOpetations = new QsvTextOperationsWidget(editor);

		highlight->setColorsDef(defColors);
		highlight->setHighlight(langDefinition);
		editor->setHighlighter(highlight);
		editor->setFrameStyle(QFrame::NoFrame);
		
		editor->findChild<QWidget*>("banner")
			->findChild<QToolButton*>("closeButton")
			->setIcon( style()->standardIcon(QStyle::SP_DockWidgetCloseButton) );
		
		QPushButton *p;
		textOpetations->initSearchWidget();
		textOpetations->m_search->findChild<QAbstractButton*>("closeButton")->setIcon( style()->standardIcon(QStyle::SP_DockWidgetCloseButton) );
		p = textOpetations->m_search->findChild<QPushButton*>("previousButton");
		p->setIcon( style()->standardIcon(QStyle::SP_ArrowUp) );
		p->setText("");
		p->setFlat(true);
		p->setAutoRepeat(true);
		p = textOpetations->m_search->findChild<QPushButton*>("nextButton");
		p->setIcon( style()->standardIcon(QStyle::SP_ArrowDown) );
		p->setText("");
		p->setFlat(true);
		p->setAutoRepeat(true);
		
		textOpetations->initReplaceWidget();
		textOpetations->m_replace->findChild<QAbstractButton*>("closeButton")->setIcon( style()->standardIcon(QStyle::SP_DockWidgetCloseButton) );
		p = textOpetations->m_search->findChild<QPushButton*>("previousButton");
		
		QToolBar *b = addToolBar( "" );
		b->addAction( tr("&New"), editor, SLOT(newDocument()))              ->setShortcut(QKeySequence("Ctrl+N"));
		b->addAction( tr("&Open"), this, SLOT(loadFile()))                  ->setShortcut(QKeySequence("Ctrl+O"));
		b->addAction( tr("&Save"), editor, SLOT(saveFile()))                ->setShortcut(QKeySequence("Ctrl+S"));
		b->addAction( tr("&Find"), textOpetations, SLOT(showSearch()))      ->setShortcut(QKeySequence("Ctrl+F"));
		b->addAction( tr("&Replace"), textOpetations, SLOT(showReplace()))  ->setShortcut(QKeySequence("Ctrl+R"));
		b->addAction( tr("Find &next"), textOpetations, SLOT(searchNext())) ->setShortcut(QKeySequence("F3"));
		b->setMovable(false);
		b->addAction( tr("Find &prev"), textOpetations, SLOT(searchPrev())) ->setShortcut(QKeySequence("Shift+F3"));
		b->setMovable(false);
		
		setCentralWidget(editor);
		showMaximized();
		editor->displayBannerMessage(tr("Click \"Open\" if you dare"));
		
		if (!file.isEmpty())
			loadFile(file);
		else {
			setWindowTitle("QtSourceView demo5 - qedit");
			QFile f("demos/demo5/readme.txt");
			if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
				return;
			editor->setPlainText(f.readAll());
			editor->removeModifications();
		}

#if 0
		e->setMarkCurrentLine(false);
		// tests for defaults
		e->setShowLineNumbers(true);
		e->setShowMargins(true);
		e->setTabSize(8);
		e->setTabIndents(true);
		e->setInsertSpacesInsteadOfTabs(true);
		e->setShowWhiteSpace(true);
#endif
	}
	
public slots:
	void loadFile( QString filename ="" )
	{
		const QString dir;
		if (filename.isEmpty()) {
			filename = QFileDialog::getOpenFileName(this,tr("Load file"),dir);
			if (filename.isEmpty())
				return;
		}
//		TODO
//		if (e->isModified){
//			e->save();
//		}
		
		editor->clear();
		langDefinition = QsvLangDefFactory::getInstanse()->getHighlight(filename);
		highlight->setHighlight(langDefinition);
		editor->loadFile(filename);
		editor->removeModifications();
		setWindowTitle(filename);
	}

private:
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w( a.arguments().count()>=2 ? a.arguments().at(1) : QString() );
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}

#include "main5.moc"
