#ifndef DATABASEUPDATER
#define DATABASEUPDATER

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QSettings>

class IUpdate {
	public:
		virtual QString fromVersion() = 0;
		virtual QString toVersion() = 0;
		virtual QString text() = 0;
		virtual bool update(QSqlDatabase *db) = 0;
};

class DatabaseUpdater {
	public:
		void update(QSettings &settings);
		void create(QSettings &settings);
	private:
		void insertUpdate(QSqlDatabase *db, QString name, QString version);
		QString getLastUpdate(QSqlDatabase *db);
		bool update0000(QSqlDatabase *db);
};

#endif // DATABASEUPDATER
