/*
 * Copyright (C) 2013 Sergey Baranov
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

/**
 * Created May 05, 2013
 *
 * @author Serge Baranov
 * @noinspection UnusedDeclaration
 */
public class KindleRunnerKindlet extends RunnerKindlet {
  private static final String CONFIG_DIR  = "/mnt/us/kindlerunner";
  private static final String CONFIG_FILE = "commands.txt";

  public void start() {
    CommandLoader loader = new CommandLoader(CONFIG_DIR, CONFIG_FILE);
    showUI(loader);
  }
}
