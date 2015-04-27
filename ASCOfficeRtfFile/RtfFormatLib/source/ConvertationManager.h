﻿#pragma once
#include "RtfReader.h"
#include "RtfDocument.h"
#include "RtfWriter.h"
#include "Writer/OOXWriter.h"
#include "Reader/OOXReader.h"


const double g_cdMaxReadRtfPercent = 0.70;
const double g_cdMaxWriteRtfPercent = 0.30;
const double g_cdMaxReadOoxPercent = 0.70;
const double g_cdMaxWriteOoxPercent = 0.30;

//#define MEMORY_SAFE_CONVERTATION


class ConvertationManager
{
public: 
	CString m_sTempFolder;

	ConvertationManager( )
	{
	}

	HRESULT ConvertRtfToOOX( std::wstring sSrcFileName, std::wstring sDstPath, CString sXMLOptions )
	{
		m_bParseFirstItem = true;

		RtfDocument oDocument;
		oDocument.m_oProperty.SetDefaultRtf();

		RtfReader oReader( oDocument, sSrcFileName );
		OOXWriter oWriter( oDocument, sDstPath );

        if (m_sTempFolder.GetLength() < 1)
            m_sTempFolder = FileSystem::Directory::GetTempPath();
		
		oReader.m_sTempFolder = FileSystem::Directory::CreateDirectoryWithUniqueName(m_sTempFolder);
		oWriter.m_sTempFolder = FileSystem::Directory::CreateDirectoryWithUniqueName(m_sTempFolder);
		
		m_poRtfReader = &oReader;
		m_poOOXWriter = &oWriter;

		m_poRtfReader->m_convertationManager = this;

		bool succes = oReader.Load( );

		//сохранение будет поэлементое в обработчике OnCompleteItemRtf
		//надо только завершить
		if( true == m_bParseFirstItem )
		{
			m_bParseFirstItem = false;
			oWriter.SaveByItemStart( );
		}
		m_poOOXWriter->SaveByItem();
		oWriter.SaveByItemEnd( );
		
		FileSystem::Directory::DeleteDirectory(oReader.m_sTempFolder);
		FileSystem::Directory::DeleteDirectory(oWriter.m_sTempFolder);

		if( true == succes )
			return S_OK;
		else 
			return S_FALSE;
	}
	HRESULT ConvertOOXToRtf( std::wstring sDstFileName, std::wstring sSrcPath, CString sXMLOptions )
	{
		m_bParseFirstItem = true;

		RtfDocument oDocument;
		oDocument.m_oProperty.SetDefaultOOX();

		OOXReader oReader( oDocument, sSrcPath );
		RtfWriter oWriter( oDocument, sDstFileName, sSrcPath );
		
        if (m_sTempFolder.GetLength() < 1)
            m_sTempFolder = FileSystem::Directory::GetTempPath();

        oReader.m_sTempFolder = FileSystem::Directory::CreateDirectoryWithUniqueName(m_sTempFolder);
		oWriter.m_sTempFolder = FileSystem::Directory::CreateDirectoryWithUniqueName(m_sTempFolder);
		
		m_poOOXReader = &oReader;
		m_poRtfWriter = &oWriter;

		m_poOOXReader->m_convertationManager = this;

		bool succes = oReader.Parse( );
		if( true == succes)
		{
			succes = oWriter.Save( );
		}

		FileSystem::Directory::DeleteDirectory(oReader.m_sTempFolder);
		FileSystem::Directory::DeleteDirectory(oWriter.m_sTempFolder);

		if( true == succes) return S_OK;
		return S_FALSE;
	}
	void OnCompleteItemRtf()
	{
		if( true == m_bParseFirstItem )
		{
			m_bParseFirstItem = false;
			m_poOOXWriter->SaveByItemStart( );
		}
		m_poOOXWriter->SaveByItem();
	}
	void OnCompleteItemOOX()
	{
		if( true == m_bParseFirstItem )
		{
			m_bParseFirstItem = false;
			m_poRtfWriter->SaveByItemStart( );
		}
		m_poRtfWriter->SaveByItem( );
	}
private:
	OOXWriter* m_poOOXWriter;
	OOXReader* m_poOOXReader;
	
	RtfWriter* m_poRtfWriter;
	RtfReader* m_poRtfReader;
	
	bool m_bParseFirstItem;


};
