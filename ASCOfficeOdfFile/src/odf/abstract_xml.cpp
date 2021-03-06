﻿/*
 * (c) Copyright Ascensio System SIA 2010-2016
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at Lubanas st. 125a-25, Riga, Latvia,
 * EU, LV-1021.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#include "abstract_xml.h"

#include "paragraph_elements.h"

#include <boost/foreach.hpp>

namespace cpdoccore { 
namespace odf_reader {


// abstract-xml
//////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t * abstract_xml::ns = L"";
const wchar_t * abstract_xml::name = L"abstract-xml";

std::wostream & abstract_xml::text_to_stream(std::wostream & _Wostream) const
{
    BOOST_FOREACH(const office_element_ptr & element, xml_content_)
    {
        element->text_to_stream(_Wostream);
    }
    return _Wostream;
}

std::wostream & abstract_xml::xml_to_stream(std::wostream & _Wostream) const
{
    BOOST_FOREACH(const office_element_ptr & element, xml_content_)
    {
        element->xml_to_stream(_Wostream);
    }
    return _Wostream;
}

void abstract_xml::add_attributes( const xml::attributes_wc_ptr & )
{
}

void abstract_xml::add_child_element( xml::sax * Reader, const std::wstring & Ns, const std::wstring & Name)
{
    CP_CREATE_ELEMENT(xml_content_);
}

void abstract_xml::add_text(const std::wstring & Text)
{
    office_element_ptr elm = text::text::create(Text) ;
    xml_content_.push_back( elm );
}

}
}
