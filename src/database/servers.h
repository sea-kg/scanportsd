#ifndef DATABASE_SERVERS
#define DATABASE_SERVERS

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QSettings>

class Servers {
	public:
		Servers(QSettings &settings);
		~Servers();
		int getID(QString ip);
		void updateIcmp(QString ip, QString icmp_status);
	private:
		QSqlDatabase *m_db;
};

#endif // DATABASE_SERVERS
