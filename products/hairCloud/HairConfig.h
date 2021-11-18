// Chongyang Ma - 2013-10-10
// Configuration about different parameters and design options
// --------------------------------------------------------------

#ifndef HAIRCONFIG_H
#define HAIRCONFIG_H

#ifndef WIN32
#include <sys/stat.h>
#endif
#include "main.h"

class CHairConfig
{
public:
	CHairConfig();
    
    CHairConfig(string fileName, bool resetFlag = false);

	bool LoadFromSynConfig(string fileName, bool resetFlag = true);

	static string AddOutputPrefix(string str);

	static void DumpTimeAndDate(FILE* file);

	static void DumpTimeAndDate(ofstream& fout);
    
    static bool DumpFinishTimeAndDate();

    static vector<string> m_vecInputPrefix;
	static string m_outputPrefix;

protected:
	virtual void ResetConfig();

	bool DumpToSynConfig();

	virtual void DumpParameters(FILE* file);

	virtual void UpdateOutputPrefix();

	void DumpStringParam(FILE* file, const char* param, const string str);
};

#endif // HAIRCONFIG_H
