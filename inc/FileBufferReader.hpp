/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:47 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/17 17:59:44 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEBUFFERREADER_HPP
#define FILEBUFFERREADER_HPP

#include <string>
#include <fstream>
#include <utility>
#include "Log.hpp"

namespace FileBuffer
{
	enum class State
	{
		UNINITIALIZED,
		INIT,
		READING,
		COMPLETE,
		ERROR
	};
}

class FileBufferReader
{
public:
	FileBufferReader();
	FileBufferReader(const std::string& _path, const size_t& _buff_size);
	~FileBufferReader();

	// copy assignemnt operator overlaod
	// FileBufferReader&	operator=(const FileBufferReader& rhs);

	// move assignemnt operator overlaod
	FileBufferReader&	operator=(FileBufferReader&& rhs);

	std::string			getNextChunk(void);
	FileBuffer::State	getState(void) const;
	size_t				getSize(void) const;
	bool				complete(void) const;
	bool				error(void) const;

private:

	void	open_fs(const std::string& _path);
	void	close_fs(void);

	FileBuffer::State	m_state;
	std::string			m_path;
	size_t				m_buff_size;
	std::fstream		m_fs;
	size_t				m_file_size;
};

#endif
