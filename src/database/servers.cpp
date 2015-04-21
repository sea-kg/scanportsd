
#include "servers.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <iostream>
#include <string.h>
#include <QUuid>
#include <QCryptographicHash>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <QVector>

// --------------------------------------------------------------------

Servers::Servers(QSettings &settings) {
	m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL", "servers_getid"));
	m_db->setHostName(settings.value("database/host").toString());
  	m_db->setDatabaseName(settings.value("database/name").toString());
    m_db->setUserName(settings.value("database/user").toString());
    m_db->setPassword(settings.value("database/pass").toString());
	// TODO port

  	if (!m_db->open()) {
		std::cerr << "[ERROR] " << m_db->lastError().text().toStdString() << "\n";
		std::cerr << "[ERROR] Failed to connect.\n";
		delete m_db;
		m_db = NULL;
  	}
}

// --------------------------------------------------------------------

Servers::~Servers() {
	if (m_db != NULL)
		m_db->close();
}

// --------------------------------------------------------------------

int Servers::getID(QString ip) {
	if (m_db == NULL)
		return 0;

	int nResult = 0;

	{
		QSqlQuery query(*m_db);
		query.prepare("SELECT id FROM servers WHERE ip = :ip");
		query.bindValue(":ip", ip);
		query.exec();

		if (query.next()) {
			 nResult = query.value(0).toInt();
		}
	}
	
	if (nResult == 0) {

		//insert
		{
			QSqlQuery query(*m_db);
			query.prepare("INSERT INTO servers (ip, dt_create, dt_change) VALUES (:ip, NOW(), NOW())");
			query.bindValue(":ip", ip);
			query.exec();
		}

		// select again
		{
			QSqlQuery query(*m_db);
			query.prepare("SELECT id FROM servers WHERE ip = :ip");
			query.bindValue(":ip", ip);
			query.exec();

			if (query.next()) {
				 nResult = query.value(0).toInt();
			}
		}
	}
	return nResult;
};

// --------------------------------------------------------------------

void Servers::updateIcmp(QString ip, QString icmp_status) {

	int nServerId = getID(ip);
	if (nServerId == 0) {
		std::cerr << "[ERROR] updateIcmp: serverid is 0\n";
		return;
	}
	
	int nLastRecID = 0;
	QString sLastStatus = "";
	{
		QSqlQuery query(*m_db);
		query.prepare("SELECT id, icmp FROM `servers_icmp` WHERE serverid = :serverid ORDER BY dt_change DESC LIMIT 0,1");
		query.bindValue(":serverid", nServerId);
		query.exec();

		if (query.next()) {
			 nLastRecID = query.value(0).toInt();
			 sLastStatus = query.value(1).toString();
		}
	}

	if (nLastRecID == 0 || sLastStatus != icmp_status) {
		QSqlQuery query(*m_db);
		query.prepare("INSERT INTO servers_icmp (serverid, icmp, dt_create, dt_change) VALUES (:serverid, :icmp, NOW(), NOW())");
		query.bindValue(":serverid", nServerId);
		query.bindValue(":icmp", icmp_status);
		query.exec();
	} else if (nLastRecID != 0 && sLastStatus == icmp_status) {
		QSqlQuery query(*m_db);
		query.prepare("UPDATE servers_icmp SET dt_change = NOW() WHERE id = :id");
		query.bindValue(":id", nLastRecID);
		query.exec();
	} else {
		// std::cout << "nLastRecID: " << nLastRecID << ", sLastStatus: " << sLastStatus.toStdString().c_str() << "\n";
	}
}
// --------------------------------------------------------------------
