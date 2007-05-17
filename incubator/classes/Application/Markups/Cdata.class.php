<?php
/***************************************************************************
 *   Copyright (C) 2007 by Ivan Y. Khvostishkov                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

	class Cdata extends SgmlType
	{
		private $data = null;
		
		/**
		 * @return Cdata
		**/
		public static function create()
		{
			return new self;
		}
		
		/**
		 * @return Cdata
		**/
		public function setData($data)
		{
			$this->data = $data;
			
			return $this;
		}
		
		public function getData()
		{
			return $this->data;
		}
	}