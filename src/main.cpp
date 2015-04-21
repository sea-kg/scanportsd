#include <QCoreApplication>
#include <QThread>
#include <QVector>
#include <QFile>
#include <QSettings>
#include <iostream>

#include "database/updater.h"
#include "database/servers.h"
#include "ping.h"

class Sleeper: public QThread
{
    public:
        static void msleep(int ms)
        {
            QThread::msleep(ms);
        }
};

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	QVector<QString> m_args;
	for(int i = 0; i < argc; i++) {
		m_args.push_back(argv[i]);
	}
	
	if (m_args.contains("--help") || m_args.size() == 1) {
		std::cout << " Usage: " << m_args[0].toStdString() << " <args>\n"
			<< "\t --help - this help \n"
			<< "\t --createdb - create new database on your mysql server by root \n"
			<< "\t --updatedb - will be update your database \n"
			<< "\t --example-config - print example \n"
			<< "\t --run - run scaning \n"
			<< "\n";
		return 1;
	}
	
	if (m_args.contains("--example-config")) {
		std::cout << "[database]\n"
			<< "host=localhost\n"
			<< "port=3306\n"
			<< "name=scanportsd\n"
			<< "user=scanportsd_u\n"
			<< "pass=scanportsd_u\n"
			<< "[scan]\n"
			<< "ip_min=93.91.166.1\n"
			<< "ip_max=93.91.166.32\n"
			<< "exclude_grey_network_addresses=false\n"
			<< "timeout_ms=5000\n"
			<< "\n";
		return 2;
	}


	QString sFileSettings = "/etc/scanportsd/config.ini";
	QFile file(sFileSettings);

	if (!file.exists()) {
	  std::cerr << "[ERROR] File '" << sFileSettings.toStdString() << "' are not exists\n";
	  return -1;
	}
	
	QSettings settings(sFileSettings, QSettings::IniFormat);
	
	QVector<QString> vOptions;
	vOptions.push_back("database/host");
	vOptions.push_back("database/name");
	vOptions.push_back("database/port");
	vOptions.push_back("database/user");
	vOptions.push_back("database/pass");
	vOptions.push_back("scan/ip_min");
	vOptions.push_back("scan/ip_max");
	vOptions.push_back("scan/exclude_grey_network_addresses");
	vOptions.push_back("scan/timeout_ms");

	// check config
	bool bCheck = true;
	for (int i = 0; i < vOptions.size(); i++) {
		if (!settings.contains(vOptions[i])) {
			bCheck = false;
			std::cerr << "[ERROR] In file '" << sFileSettings.toStdString() << "' are not contains option " + vOptions[i].toStdString() + " \n";
		}
	}

	if (bCheck == false) {
		return -2;
	}

	int ip0_min = 1;
	int ip0_max = 254;
	int ip1_min = 1;
	int ip1_max = 254;
	int ip2_min = 1;
	int ip2_max = 254;
	int ip3_min = 1;
	int ip3_max = 254;
	
	{
		QStringList list = settings.value("scan/ip_min").toString().split(".", QString::SkipEmptyParts);
		if (list.size() != 4) {
			std::cerr << "[ERROR] Option 'scan/ip_min' has incorrect format \n";
			return -3;
		}
		ip0_min = list[0].toInt();
		ip1_min = list[1].toInt();
		ip2_min = list[2].toInt();
		ip3_min = list[3].toInt();
		
		// TODO check 1 <= ip0_min <= 254
		// TODO check 1 <= ip1_min <= 254
		// TODO check 1 <= ip2_min <= 254
		// TODO check 1 <= ip3_min <= 254
	}

	{
		QStringList list = settings.value("scan/ip_max").toString().split(".", QString::SkipEmptyParts);
		if (list.size() != 4) {
			std::cerr << "[ERROR] Option 'scan/ip_max' has incorrect format \n";
			return -4;
		}
		ip0_max = list[0].toInt();
		ip1_max = list[1].toInt();
		ip2_max = list[2].toInt();
		ip3_max = list[3].toInt();
		
		// TODO check 1 <= ip0_max <= 254
		// TODO check 1 <= ip1_max <= 254
		// TODO check 1 <= ip2_max <= 254
		// TODO check 1 <= ip3_max <= 254
	}
	
	std::cout << "Will be scan addresses: "
		<< QString::number(ip0_min).toStdString() << "."
		<< QString::number(ip1_min).toStdString() << "."
		<< QString::number(ip2_min).toStdString() << "."
		<< QString::number(ip3_min).toStdString()
		<< " - "
		<< QString::number(ip0_max).toStdString() << "."
		<< QString::number(ip1_max).toStdString() << "."
		<< QString::number(ip2_max).toStdString() << "."
		<< QString::number(ip3_max).toStdString()
		<< "\n";

	bool bExcludeGreyNetworkAddresses = settings.value("scan/exclude_grey_network_addresses", true).toBool();
	std::cout << "Exclude Grey Network Addresses = " << (bExcludeGreyNetworkAddresses ? "ON" : "OFF") << "\n";
	int timeout_ms = settings.value("scan/timeout_ms", true).toInt();
	std::cout << "timeout = " << timeout_ms << "ms\n";
	
	
	if (m_args.contains("--createdb")) {
		DatabaseUpdater *du = new DatabaseUpdater();
		du->create(settings);
		return 3;
	}
	
	if (m_args.contains("--updatedb")) {
		DatabaseUpdater *du = new DatabaseUpdater();
		du->update(settings);
		return 3;
	}

	if (m_args.contains("--run")) {
		Servers *pServers = new Servers(settings);
		while(1) {
			for (int i0 = ip0_min; i0 <= ip0_max; i0++) {
				for (int i1 = ip1_min; i1 <= ip1_max; i1++) {
					
					if (bExcludeGreyNetworkAddresses) {
						if (i0 == 192 && i1 == 168)
							continue; // skip 192.168.0.0/16
						if (i0 == 172 && i1 >= 16 && i1 <= 31)
							continue; // skip 172.16.0.0/12
						if (i0 == 10)
							continue; // skip 10.0.0.0/8
					}

					for (int i2 = ip2_min; i2 <= ip2_max; i2++) {
						for (int i3 = ip3_min; i3 <= ip3_max; i3++) {
							QString ip = QString::number(i0) + "." + QString::number(i1) + "." + QString::number(i2) + "." + QString::number(i3);
							std::cout << "IP: " << ip.toStdString() << " \t ";
							if (ping(ip)) {
								std::cout << "UP";
								pServers->updateIcmp(ip, "UP");
							} else {
								std::cout << "DOWN";
								pServers->updateIcmp(ip, "DOWN");
							}
							std::cout << "\n";
							// return -100;
						}
					}	
				}			
			}
			Sleeper::msleep(timeout_ms);
		}
		delete pServers;
		return 3;
	}
}
