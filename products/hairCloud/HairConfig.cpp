#include "HairConfig.h"

#ifndef WIN32
#define MAX_PATH 260
#endif

// Default values...
string CHairConfig::m_outputPrefix;
vector<string> CHairConfig::m_vecInputPrefix;

CHairConfig::CHairConfig()
{
}

CHairConfig::CHairConfig(string fileName, bool resetFlag /* =false */)
{
    LoadFromSynConfig(fileName, resetFlag);
}

bool CHairConfig::LoadFromSynConfig(string fileName, bool resetFlag /* = true */)
{
	ifstream fin(fileName.c_str());
	if ( fin.fail() == true )
	{
		cout << "Failed to load config parameters from config file " << fileName << "!\n";
		return false;
	}
    m_vecInputPrefix.clear();
	string param;
	while ( fin >> param )
	{
		if ( param == string("OUTPUT_PREFIX") )
		{
			fin >> m_outputPrefix;
		}
	}
	if ( resetFlag == false )
	{
		return true;
	}
	ResetConfig();

	return true;
}

string CHairConfig::AddOutputPrefix(string str)
{
	if ( m_outputPrefix.empty() == true )
	{
		return str;
	}
	ostringstream oss;
	oss << m_outputPrefix << str;
	string strNew = oss.str();
	return strNew;
}

void CHairConfig::ResetConfig()
{
	if ( m_outputPrefix.empty() == false )
	{
		UpdateOutputPrefix();
	}
	DumpToSynConfig();
}

bool CHairConfig::DumpToSynConfig()
{
	ostringstream oss;
	oss << m_outputPrefix << "SynConfig.txt";
	string fileName = oss.str();
	FILE* file;
	file = fopen(fileName.c_str(), "w");
	if ( !file )
	{
		cout << "Failed to dump parameters into config file " << fileName << "!\n";
		return false;
	}
	DumpTimeAndDate(file);
	DumpParameters(file);
	fclose(file);

	return true;
}

void CHairConfig::DumpTimeAndDate(FILE* file)
{
	time_t myTime = time(NULL);
	tm* ptrTime = localtime(&myTime);
	fprintf(file, "%02d:%02d:%02d ", ptrTime->tm_hour, ptrTime->tm_min, ptrTime->tm_sec);
	fprintf(file, "%02d/%02d/%04d\n\n", ptrTime->tm_mon+1, ptrTime->tm_mday, ptrTime->tm_year+1900);
}

void CHairConfig::DumpTimeAndDate(ofstream& fout)
{
	time_t myTime = time(NULL);
	tm* ptrTime = localtime(&myTime);
	char str[1000];
	sprintf(str, "%02d:%02d:%02d ", ptrTime->tm_hour, ptrTime->tm_min, ptrTime->tm_sec);
	fout << str;
	sprintf(str, "%02d/%02d/%04d", ptrTime->tm_mon+1, ptrTime->tm_mday, ptrTime->tm_year+1900);
	fout << str << endl;
}

bool CHairConfig::DumpFinishTimeAndDate()
{
    ostringstream oss;
	oss << m_outputPrefix << "SynConfig.txt";
	string fileName = oss.str();
	FILE* file;
	file = fopen(fileName.c_str(), "a+");
	if ( !file )
	{
		cout << "Failed to dump finish time and date into config file " << fileName << "!\n";
		return false;
	}
    DumpTimeAndDate(file);
    fclose(file);

    return true;
}

void CHairConfig::DumpParameters(FILE* file)
{
	DumpStringParam(file, "OUTPUT_PREFIX", m_outputPrefix);
    for ( int i=0; i<int(m_vecInputPrefix.size()); i++ )
    {
        DumpStringParam(file, "VECTOR_INPUT_PREFIX", m_vecInputPrefix[i]);
    }
}

void CHairConfig::UpdateOutputPrefix()
{
    while ( m_outputPrefix.empty() == false && isdigit(m_outputPrefix[m_outputPrefix.size()-1]) == 0 )
    {
        m_outputPrefix = m_outputPrefix.substr(0, m_outputPrefix.size()-1);
    }
#ifdef WIN32
	m_outputPrefix = m_outputPrefix + string("\\");
#else
    m_outputPrefix = m_outputPrefix + string("/");
#endif
	ostringstream oss;
#ifdef WIN32
	bool flag = CreateDirectoryA(m_outputPrefix.c_str(), NULL);
	const int numLength = 2;
	while ( flag == false && m_outputPrefix.size() >= 2 )
	{
		string subStr = m_outputPrefix.substr(m_outputPrefix.length()-numLength-1, 2);
		int num = atoi(subStr.c_str()) + 1;
		char numChar[MAX_PATH];
		sprintf(numChar, "%02d", num);
		ostringstream oss;
		oss << m_outputPrefix.substr(0, m_outputPrefix.length()-numLength-1) << numChar << "\\";
		m_outputPrefix = oss.str();
		flag = CreateDirectoryA(m_outputPrefix.c_str(), NULL);
	}
	cout << "Dump results to the directory: " << m_outputPrefix.c_str() << endl;
#else
    string outputFolder = m_outputPrefix.substr(0, m_outputPrefix.size()-1);
    int flag = mkdir(outputFolder.c_str(), 0777);
	const int numLength = 2;
	while ( flag == -1 && m_outputPrefix.size() >= 2 )
	{
		string subStr = m_outputPrefix.substr(m_outputPrefix.length()-numLength-1, 2);
		int num = atoi(subStr.c_str()) + 1;
		char numChar[MAX_PATH];
		sprintf(numChar, "%02d", num);
		ostringstream oss;
		oss << m_outputPrefix.substr(0, m_outputPrefix.length()-numLength-1) << numChar << "/";
		m_outputPrefix = oss.str();
        outputFolder = m_outputPrefix.substr(0, m_outputPrefix.size()-1);
		flag = mkdir(outputFolder.c_str(), 0777);
	}
	cout << "Dump results to the directory: " << m_outputPrefix.c_str() << endl;
#endif
}

void CHairConfig::DumpStringParam(FILE* file, const char* param, const string str)
{
	if ( str.empty() != true )
	{
		fprintf(file, "%s\t%s\n", param, str.c_str());
	}
	else
	{
		//fprintf(file, "%s\t%s\n", param, "NULL");
	}
}
