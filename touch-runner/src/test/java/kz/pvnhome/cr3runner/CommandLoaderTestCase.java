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

import junit.framework.TestCase;

import java.util.List;

/**
 * Created Jan 22, 2012 2:05:27 PM
 *
 * @author Victor Pyankov
 */
public class CommandLoaderTestCase extends TestCase {

  /**
   * Test method for {@link kz.pvnhome.cr3runner.CommandLoader#load()}.
   */
  public void testLoad() {
    CommandLoader loader = new CommandLoader("conf/touchrunner", "commands.txt");

    List commands = loader.load();

    assertEquals("count", 3, commands.size());

    check("1", commands.get(0), "/mnt/us/cr3", "goqt.sh", "cr3", "Cool Reader 3");
    check("2", commands.get(1), "/mnt/us/prog1", "command1", "-param", "Program 1");
    check("3", commands.get(2), "/mnt/us/prog2", "command2", "", "Program 2");
  }

  private static void check(String name, Object obj, String path, String cmd, String param, String description) {
    Command command = (Command) obj;
    assertEquals("path" + name, path, command.getPath());
    assertEquals("command" + name, cmd, command.getCommand());
    assertEquals("param" + name, param, command.getParam());
    assertEquals("desc" + name, description, command.getDescription());
  }
}
