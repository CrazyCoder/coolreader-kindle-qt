/*
 * Copyright (C) 2012 Victor Pyankov
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
package kz.pvnhome.cr3runner;

import com.amazon.kindle.kindlet.KindletContext;

/**
 * Kindle Touch version
 *
 * @author Victor Pyankov & Serge Baranov
 * @noinspection UnusedDeclaration
 */
public class CoolReaderRunnerKindlet extends RunnerKindlet {
  private static final String CONFIG_DIR  = "/mnt/us/cr3";
  private static final String CONFIG_FILE = "commands.txt";

  protected void initCommandsUI(KindletContext ctx) {
    CommandLoader loader = new CommandLoader(CONFIG_DIR, CONFIG_FILE);
    showUI(ctx, loader);
  }
}
