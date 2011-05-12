#ifndef HIGHLIGHTDEFINITIONMANAGER_H
#define HIGHLIGHTDEFINITIONMANAGER_H

#include "highlightdefinition.h"
#include "highlightdefinitionmanager.h"

#include <QString>
#include <QSharedPointer>
#include <QFileInfo>
#include <QHash>
#include <QMultiHash>

namespace Core{
class MimeDatabase;
}

namespace TextEditor {
namespace Internal {

class HighlightDefinitionManager
{
public:
	static HighlightDefinitionManager *instance();
	const QString definitionIdByName(const QString &externalName);
	bool isBuildingDefinition(const QString &externalName);
	QSharedPointer<HighlightDefinition> definition(const QString &externalName);
	void parseDefinitionMetadata(const QFileInfo &fileInfo,
				     QString *comment,
				     QStringList *mimeTypes,
				     QStringList *patterns);
	
	void buildMimeDatabase( const QString &originDir );
	QString getHighlightsPath();
	
	
	void setMimeDatabase( Core::MimeDatabase *mime ){ m_mime = mime; }
	Core::MimeDatabase* getMimeDatabase(){ return m_mime; }
	
private:
	HighlightDefinitionManager();
	static HighlightDefinitionManager *m_instance;
	Core::MimeDatabase *m_mime;
	
//	PriorityCompare m_priorityComp;
	QHash<QString, QString> m_idByName;
	QMultiHash<QString, QString> m_idByMimeType;
	QHash<QString, QSharedPointer<HighlightDefinition> > m_definitions;
};


}
}

#endif // HIGHLIGHTDEFINITIONMANAGER_H