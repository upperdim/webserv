/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:45 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/17 18:05:59 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileBufferReader.hpp"

FileBufferReader::FileBufferReader()
	:	m_state(FileBuffer::State::UNINITIALIZED),
		m_path(""),
		m_buff_size(0),
		m_fs(),
		m_file_size(0)
{
}

FileBufferReader::FileBufferReader(const std::string& _path, const size_t& _buff_size)
	:	m_state(FileBuffer::State::INIT),
		m_path(_path),
		m_buff_size(_buff_size),
		m_fs(),
		m_file_size(0)
{
	open_fs(_path);
}

FileBufferReader::~FileBufferReader()
{
	close_fs();
}


/* ************************************************************************** */
/* ************************************************************************** */


// copy assignemnt operator overlaod
// FileBufferReader&	FileBufferReader::operator=(const FileBufferReader& rhs)
// {
// 	if (this != &rhs)
// 	{
// 		close_fs();
// 		m_state		= rhs.m_state;
// 		m_path		= rhs.m_path;
// 		m_buff_size	= rhs.m_buff_size;
// 		m_file_size	= 0;
// 		open_fs(m_path);
// 	}
// 	return (*this);
// }

// move assignemnt operator overlaod
FileBufferReader&	FileBufferReader::operator=(FileBufferReader&& rhs)
{
	if (this != &rhs)
	{
		m_state		= rhs.m_state;
		m_path		= std::move(rhs.m_path);
		m_buff_size	= rhs.m_buff_size;
		m_fs		= std::move(rhs.m_fs);
		m_file_size	= rhs.m_file_size;
	}
	return (*this);
}

std::string	FileBufferReader::getNextChunk(void)
{
	if (m_state != FileBuffer::State::READING)
		return ("");

	std::string	content(m_buff_size, '\0');
	m_fs.read(&content[0], m_buff_size);
	if (m_fs.bad())
		m_state = FileBuffer::State::ERROR;
	else if (m_fs.eof())
		m_state = FileBuffer::State::COMPLETE;
	
	content.resize(m_fs.gcount());
	return (content);
}

FileBuffer::State	FileBufferReader::getState(void) const
{
	return (m_state);
}

size_t	FileBufferReader::getSize(void) const
{
	return (m_file_size);
}

bool	FileBufferReader::complete(void) const
{
	return (m_state == FileBuffer::State::COMPLETE);
}

bool	FileBufferReader::error(void) const
{
	return (m_state == FileBuffer::State::ERROR);
}


/* ************************************************************************** */
/* ************************************************************************** */

void	FileBufferReader::open_fs(const std::string& _path)
{
	if (m_state == FileBuffer::State::UNINITIALIZED
		|| m_state == FileBuffer::State::ERROR)
		return ;

	m_fs.open(_path, std::ios::in | std::ios::binary);
	if (!m_fs.is_open())
	{
		m_state = FileBuffer::State::ERROR;
		return ;
	}

	// get the file size
    m_fs.seekg (0, m_fs.end);
	if (m_fs.fail())
	{
		m_state = FileBuffer::State::ERROR;
		m_fs.close();
		return ;
	}
    m_file_size = m_fs.tellg();
    m_fs.seekg (0, m_fs.beg);
	if (m_fs.fail())
	{
		m_state = FileBuffer::State::ERROR;
		m_fs.close();
		return ;
	}

	m_state = FileBuffer::State::READING;
}

void	FileBufferReader::close_fs(void)
{
	if (m_fs.is_open())
	{
		LOG_WARNING_LM("CLOSING", std::string("FileBufferReader ") + m_path);
		m_file_size = 0;
		m_fs.close();
	}
}
