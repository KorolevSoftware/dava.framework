/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __DAVAENGINE_LOCALIZATION_SYSTEM_H__
#define __DAVAENGINE_LOCALIZATION_SYSTEM_H__

#include "Base/BaseTypes.h"
#include "Base/Singleton.h"

namespace DAVA 
{

class LocalizationSystem : public Singleton<LocalizationSystem>
{
public:	
	LocalizationSystem();
	virtual ~LocalizationSystem();
	
	void InitWithDirectory(const String &directoryPath);
	
	const String &GetCurrentLocale();
	void SetCurrentLocale(const String &newLangId);
	
	const WideString & GetLocalizedString(const WideString & key);

	struct YamlDataHolder
	{
		uint32 fileSize;
		uint32 dataOffset;
		uint8 * data;
	};
	
private:

	void LoadStringFile(const String & fileName);
	void UnloadStringFile(const String & fileName); 

	String langId;
	
	struct StringFile
	{
		String pathName;
		Map<WideString, WideString> strings;
	};
	List<StringFile*> stringsList;

	StringFile* LoadFromYamlFile(const String & fileName);

	YamlDataHolder dataHolder;
};

inline const WideString &  LocalizedString(const WideString & key)
{
	return LocalizationSystem::Instance()->GetLocalizedString(key);
}
inline const WideString &  LocalizedString(const String & key)
{
	return LocalizationSystem::Instance()->GetLocalizedString(WideString(key.begin(), key.end()));
}
	
	
};

#endif // __DAVAENGINE_LOCALIZATION_SYSTEM_H__