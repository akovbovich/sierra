#pragma once

/****************************************************************************/
// c_SCString

class SCString
{
	friend class c_Graph;

	private:
		char* m_String;
		int m_OwnString;
		int m_IsModified;
		const char* m_DefaultString;
		char Reserve[60];

	public:
		SCString();
		SCString(int DummyValue);  // this is needed by the c_ArrayWrapper class
		SCString(const SCString& Str);//Copy constructor
		SCString(const char* SourceString);
		SCString(const char* SourceString, int Length);
		SCString(const char ch);
		~SCString();

		void Initialize();

		bool operator == (const SCString& Rhs) const;
		bool operator == (const char* Rhs) const;
		bool operator != (const SCString& Rhs) const;
		bool operator != (const char* Rhs) const;
		bool operator < (const SCString& Rhs) const;
		bool operator < (const char* Rhs) const;
		SCString& operator = (const SCString& Rhs);
		SCString& operator = (const char* String);
		SCString& operator += (const SCString& Rhs);
		SCString operator + (const SCString& Rhs) const;

		operator const char* () const;

		SCString& Format(const char* String, ...);

		int Compare(const char* str, int NumChars = 0) const;
		int CompareNoCase(const char* str, int NumChars = 0) const;
		int CompareNoCase(const SCString& str, int NumChars = 0) const;

		int IsModified() const;

		const char* GetChars() const;
		size_t GetLength() const;

		int IndexOf(char Delimiter, int StartIndex = 0) const;
		int LastIndexOf(char symbol, unsigned int StartIndex) const;

		SCString GetSubString(unsigned int Count, unsigned int StartIndex = 0) const;

		void ParseLines(std::vector <SCString> &Lines);
		void ParseLineItemsAsFloats(std::vector <float> &FloatValues);

		int Tokenize(const char* Delim, std::vector<char*>& Tokens);

		SCString& Append(const SCString& Rhs);

		SCString Left(int Count) const;
		SCString Right(int Count) const;

	private:
		static char* StringNew(size_t NumBytes);
		static void StringDelete(char* String);

		SCString& Copy(const char* String, size_t StringLength);

		// For Sierra Chart internal use only
		void InternalSetStatic(char* String);
};

/*==========================================================================*/
inline SCString::SCString()
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString = 0;
	m_IsModified = 0;

}

/*============================================================================
	This constructor is needed by the c_ArrayWrapper class.
----------------------------------------------------------------------------*/
inline SCString::SCString(int DummyValue)
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString= 0;
	m_IsModified= 0;

}

/*==========================================================================*/
inline SCString::SCString(const SCString& Str)
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString = 0;
	m_IsModified = Str.m_IsModified;

	size_t StringLength =0;
	if (Str.m_String)
		StringLength =strlen(Str.m_String);

	if (StringLength != 0)
	{
		m_String = StringNew(StringLength + 1);
		if (m_String != NULL)
		{
			#if __STDC_WANT_SECURE_LIB__
			strcpy_s(m_String, StringLength + 1, Str.m_String);
			#else
			strcpy(m_String, Str.m_String);
			#endif
			m_OwnString = 1;
		}
		else
		{
			m_String = (char*)m_DefaultString; //NULL
		}
	}
	else
	{
		m_String = (char*)m_DefaultString; //NULL
	}
}

/*==========================================================================*/
inline SCString::SCString(const char* SourceString)
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString= 0;
	m_IsModified= 0;


	size_t StringLength = 0;
	if (SourceString != NULL)
		StringLength = strlen(SourceString);

	if (StringLength != 0)
	{
		size_t BufferSize = StringLength + 1;

		m_String = StringNew(BufferSize);
		if (m_String != NULL)
		{
			#if __STDC_WANT_SECURE_LIB__
			strncpy_s(m_String, BufferSize, SourceString, StringLength);
			#else
			strncpy(m_String, SourceString, StringLength);
			#endif
			m_OwnString = 1;
			m_IsModified = 1;
		}
		else
		{
			m_String = (char*)m_DefaultString; //NULL
		}
	}
	else
		m_String = (char*)m_DefaultString; //NULL

}

/*==========================================================================*/
inline SCString::SCString(const char* SourceString, int Length)
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString= 0;
	m_IsModified= 0;


	size_t StringLength = 0;
	if (SourceString != NULL)
	{
		if (Length >= 0)
			StringLength = Length;
		else
			StringLength = strlen(SourceString);
	}

	if (StringLength != 0)
	{
		size_t BufferSize = StringLength + 1;

		m_String = StringNew(BufferSize);
		if (m_String != NULL)
		{
			#if __STDC_WANT_SECURE_LIB__
			strncpy_s(m_String, BufferSize, SourceString, StringLength);
			#else
			strncpy(m_String, SourceString, StringLength);
			#endif
			m_OwnString = 1;
			m_IsModified = 1;
		}
		else
		{
			m_String = (char*)m_DefaultString; //NULL
		}
	}
	else
		m_String = (char*)m_DefaultString; //NULL

}

/*==========================================================================*/
inline SCString::SCString(const char ch)
{
	m_DefaultString = "";
	m_String = (char*)m_DefaultString; //NULL
	m_OwnString= 0;
	m_IsModified= 0;


	size_t StringLength = 1;
	m_String = StringNew(StringLength + 1);
	if (m_String != NULL)
	{
		m_String[0] = ch;
		m_String[StringLength] = '\0';
		m_OwnString = 1;
		m_IsModified = 1;
	}
	else
		m_String = (char*)m_DefaultString; //NULL

}

/*==========================================================================*/
inline SCString::~SCString()
{
	if (m_OwnString && m_String != NULL)
		StringDelete(m_String);
}

/*==========================================================================*/
inline void SCString::Initialize()
{
	if (m_OwnString && m_String != NULL)
		StringDelete(m_String);


	m_String = (char*)m_DefaultString; //NULL
	m_OwnString = 0;
	m_IsModified = 0;
}

/*==========================================================================*/
inline bool SCString::operator == (const SCString& Rhs) const
{
	return (Compare(Rhs.GetChars(), Rhs.GetLength()) == 0);
}

/*==========================================================================*/
inline bool SCString::operator == (const char* Rhs) const
{
	return (Compare(Rhs) == 0);
}

/*==========================================================================*/
inline bool SCString::operator != (const SCString& Rhs) const
{
	return (Compare(Rhs.GetChars(), Rhs.GetLength()) != 0);
}

/*==========================================================================*/
inline bool SCString::operator != (const char* Rhs) const
{
	return (Compare(Rhs) != 0);
}

/*==========================================================================*/
inline bool SCString::operator < (const SCString& Rhs) const
{
	return Compare(Rhs.GetChars(), Rhs.GetLength()) < 0;
}

/*==========================================================================*/
inline bool SCString::operator < (const char* Rhs) const
{
	return Compare(Rhs) < 0;
}

/*==========================================================================*/
inline SCString& SCString::operator = (const SCString& Rhs)
{
	if (&Rhs == this)
		return *this;

	if (Rhs.m_String)
		Copy(Rhs.m_String, strlen(Rhs.m_String));
	else
		Initialize();

	m_IsModified = 1;

	return *this;
}

/*==========================================================================*/
inline SCString& SCString::operator = (const char* String)
{
	if (String)
		Copy(String, strlen(String));
	else
		Initialize();

	m_IsModified = 1;

	return *this;
}

/*==========================================================================*/
inline SCString& SCString::operator += (const SCString& Rhs)
{
	return Append(Rhs);
}

/*==========================================================================*/
inline SCString SCString::operator + (const SCString& Rhs) const
{
	SCString Result(*this);
	Result += Rhs;
	return Result;
}

/*==========================================================================*/
inline SCString::operator const char* () const
{
	if (m_String)
		return m_String;
	else
		return "";
}

/*==========================================================================*/
inline SCString& SCString::Format(const char* String, ...)
{
	Initialize();

	va_list ArgList;
	va_start(ArgList, String);

	const size_t MAX_STRING_LENGTH = 500;

	m_String = StringNew(MAX_STRING_LENGTH + 1);
	if (m_String != NULL)
	{
		#if __STDC_WANT_SECURE_LIB__
		_vsnprintf_s(m_String, MAX_STRING_LENGTH + 1, MAX_STRING_LENGTH, String, ArgList);
		#else
		_vsnprintf(m_String, MAX_STRING_LENGTH, String, ArgList);
		#endif
		m_OwnString = 1;
	}
	else
	{
		m_String = (char*)m_DefaultString;
	}

	va_end(ArgList);

	m_IsModified = 1;

	return *this;
}

/*==========================================================================*/
inline int SCString::Compare(const char* str, int NumChars) const
{
	const char* InternalString= ""; 
	if (m_String)
		InternalString=m_String;

	if (str )
	{
		if (NumChars == 0)
			return strcmp(str, InternalString);
		else
			return strncmp(str, InternalString, NumChars);
	}
	else 
		return 0;

}

/*==========================================================================*/
inline int SCString::CompareNoCase(const char* str, int NumChars) const
{
	const char* InternalString= ""; 
	if (m_String)
		InternalString=m_String;

	#if _MSC_VER >= 1400
	if (NumChars == 0)
		return _stricmp(str, InternalString);
	else
		return _strnicmp(str, InternalString, NumChars);
	#else
	if (NumChars == 0)
		return stricmp(str, InternalString);
	else
		return strnicmp(str, InternalString, NumChars);
	#endif
}

/*==========================================================================*/
inline int SCString::CompareNoCase(const SCString& str, int NumChars) const
{
	return CompareNoCase(str.GetChars(),NumChars);
}

/*==========================================================================*/
inline int SCString::IsModified() const
{
	return m_IsModified;
}

/*==========================================================================*/
inline const char* SCString::GetChars() const
{
	if (m_String)
		return m_String;
	else
		return "";
}

/*==========================================================================*/
inline size_t SCString::GetLength() const
{
	if (m_String == NULL)
		return 0;
	else
		return strlen(m_String);
}

/*==========================================================================*/
inline int SCString::IndexOf(char Delimiter, int StartIndex) const
{
	if (m_String == NULL)
		return -1;

	for (int i = StartIndex; i < (int)GetLength(); i++)
	{
		if (m_String[i] == Delimiter)
			return i;
	}
	return -1;
}

/*==========================================================================*/
inline int SCString::LastIndexOf(char symbol, unsigned int StartIndex) const
{
	if (m_String == NULL)
		return -1;

	size_t Length = GetLength();

	if (StartIndex >= Length)
		StartIndex = Length - 1;

	for (int i = StartIndex; i >= 0; i--)
	{
		if (m_String[i] == symbol)
			return i;
	}

	return -1;
}

/*==========================================================================*/
inline SCString SCString::GetSubString(unsigned int Count, unsigned int StartIndex) const
{
	if (m_String == NULL)
		return SCString();

	size_t Length = GetLength();
	if (StartIndex >= Length)
		return SCString();

	if (StartIndex + Count > Length)
		Count = Length - StartIndex;

	SCString Result;
	Result.Copy(m_String + StartIndex, Count);

	return Result;
}

/*==========================================================================*/
inline void SCString::ParseLines(std::vector <SCString>& Lines)
{

	unsigned int StartIndex = 0;
	while(true)
	{
		int Pos = IndexOf('\n', StartIndex);


		if (Pos == -1)
			Pos = GetLength(); 

		SCString Line;
		Line = GetSubString(Pos - StartIndex, StartIndex);
		Lines.push_back(Line);
		StartIndex = Pos + 1;

		if(StartIndex >= GetLength())
			break;

	}
}

/*==========================================================================*/
inline void SCString::ParseLineItemsAsFloats(std::vector<float>& FloatValues)
{
	char FieldsDelimitter = ',';

	int find_delim = IndexOf(FieldsDelimitter);
	unsigned int pos = 0;

	int ArraySize = 1000;
	int CurrentItem = 0;

	while (find_delim > 0 && CurrentItem < ArraySize)
	{
		SCString NextStr = GetSubString(find_delim - pos, pos);

		FloatValues.push_back((float)atof(NextStr));
		pos = find_delim + 1;
		find_delim = IndexOf(FieldsDelimitter,find_delim + 1);

		CurrentItem++;
	}
}

/*==========================================================================*/
inline int SCString::Tokenize(const char* Delim, std::vector<char*>& Tokens)
{
	Tokens.erase(Tokens.begin(), Tokens.end());

	if (m_String == NULL)
		return 0;

	Tokens.push_back(m_String);

	if (Delim == NULL)
		return 1;

	int StrLen = GetLength();
	int DelimLen = strlen(Delim);

	if (DelimLen == 0)
		return 1;  // Not worth splitting every character

	for (int a = 0; a < StrLen; a++)
	{
		// Look for the delimiter
		bool DelimFound = true;
		for (int b = 0; b < DelimLen; b++)
		{
			if (a + b >= StrLen)
			{
				DelimFound = false;
				break;
			}

			if (m_String[a + b] != Delim[b])
			{
				DelimFound = false;
				break;
			}
		}

		if (DelimFound)
		{
			m_String[a] = '\0';
			Tokens.push_back(m_String + a + DelimLen);
			a += DelimLen - 1;
		}
	}

	return Tokens.size();
}

/*==========================================================================*/
inline SCString& SCString::Append(const SCString& Rhs)
{
	size_t RhsLength = Rhs.GetLength();
	if (RhsLength == 0)
		return *this;

	size_t OldStringLength = GetLength();

	size_t NewStringLength = OldStringLength + RhsLength;
	char* NewString = StringNew(NewStringLength + 1);
	if (NewString)
	{
		const char*InternalString = "";
		if (m_String)
			InternalString =m_String;

		#if __STDC_WANT_SECURE_LIB__
		strncpy_s(NewString, NewStringLength + 1, InternalString, OldStringLength);
		strncpy_s(NewString + OldStringLength, NewStringLength + 1 - OldStringLength, Rhs.m_String, RhsLength);
		#else
		strncpy(NewString, InternalString, OldStringLength);
		strncpy(NewString + OldStringLength, Rhs.m_String, RhsLength);
		#endif
		NewString[NewStringLength] = '\0';

		Initialize();
		m_String = NewString;
		m_OwnString = 1;
	}

	m_IsModified = 1;

	return *this;
}

/*==========================================================================*/
inline SCString SCString::Left(int Count) const
{
	int StringLength = GetLength();

	if (Count >= StringLength)
		return *this;
	else if (Count > 0)
		return SCString(m_String, Count);
	else if (Count < 0)
	{
		int TargetLength = StringLength + Count;
		if (TargetLength <= 0)
			return SCString();
		else
			return SCString(m_String, TargetLength);
	}
	else
		return SCString();
}

/*==========================================================================*/
inline SCString SCString::Right(int Count) const
{
	int StringLength = GetLength();

	if (Count >= StringLength)
		return *this;
	else if (Count > 0)
		return SCString(m_String + (StringLength - Count), Count);
	else if (Count < 0)
	{
		int TargetLength = StringLength + Count;
		if (TargetLength <= 0)
			return SCString();
		else
			return SCString(m_String + (StringLength - TargetLength), TargetLength);
	}
	else
		return SCString();
}

/*============================================================================
	Static
----------------------------------------------------------------------------*/
inline char* SCString::StringNew(size_t NumBytes)
{
	char* p_Block;

	p_Block = (char*)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)NumBytes);
	memset(p_Block,0,NumBytes);
	return p_Block;
}

/*============================================================================
	Static
----------------------------------------------------------------------------*/
inline void SCString::StringDelete(char* String)
{
	if (String != NULL)
		HeapFree(GetProcessHeap(), 0, String);
}

/*==========================================================================*/
inline SCString& SCString::Copy(const char* String, size_t StringLength)
{
	if (String == NULL)
		return *this;

	Initialize();

	if (StringLength != 0)
	{
		#if __STDC_WANT_SECURE_LIB__
		m_String = StringNew(StringLength + 1);
		#else
		m_String = StringNew(StringLength + 1 + 4);  // 4 bytes additional for safety
		#endif
		if (m_String == NULL)
			m_String = (char*)m_DefaultString;
		else
		{
			#if __STDC_WANT_SECURE_LIB__
			strncpy_s(m_String, StringLength + 1, String, StringLength);
			#else
			strncpy(m_String, String, StringLength);
			#endif
			m_String[StringLength] = '\0';
			m_OwnString = 1;
		}
	}

	m_IsModified = 1;

	return *this;
}

/*============================================================================
	For Sierra Chart internal use only.
----------------------------------------------------------------------------*/
inline void SCString::InternalSetStatic(char* String)
{
	Initialize();

	m_String = String;
	m_OwnString = 0;
	m_IsModified = 0;
}

/*==========================================================================*/
