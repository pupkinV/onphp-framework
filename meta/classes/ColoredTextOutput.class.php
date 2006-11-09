<?php
/***************************************************************************
 *   Copyright (C) 2006 by Konstantin V. Arkhipov                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

	/**
	 * @ingroup MetaBase
	**/
	final class ColoredTextOutput extends TextOutput
	{
		public function setMode(
			$attribute = ConsoleMode::ATTR_RESET_ALL,
			$foreground = ConsoleMode::FG_WHITE,
			$background = ConsoleMode::BG_BLACK
		)
		{
			echo
				chr(0x1B)
				.'['.$attribute.';'
				.$foreground.';'
				.$background.'m';
			
			return $this;
		}
		
		public function resetAll()
		{
			echo chr(0x1B).'[0m';
			
			return $this;
		}
	}
?>