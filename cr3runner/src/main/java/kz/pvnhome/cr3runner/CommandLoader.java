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

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

/**
 * Created Jan 22, 2012 1:55:37 PM
 *
 * @author Victor Pyankov
 */
public class CommandLoader {
  private String path;
  private String fileName;

  public CommandLoader() {
  }

  public CommandLoader(String path, String fileName) {
    this.path = path;
    this.fileName = fileName;
  }

  public void setPath(String path) {
    this.path = path;
  }

  public void setFileName(String fileName) {
    this.fileName = fileName;
  }

  public List load() {
    ArrayList commands = new ArrayList();

    try {
      InputStream in = new FileInputStream(new File(path + File.separator + fileName));
      try {
        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
        String line;
        while ((line = reader.readLine()) != null) {
          StringTokenizer st = new StringTokenizer(line, ";");
          if (st.countTokens() == 4) {
            Command cmd = new Command();
            commands.add(cmd);
            cmd.setPath(nvl(st.nextToken()));
            cmd.setCommand(nvl(st.nextToken()));
            cmd.setParam(nvl(st.nextToken()));
            cmd.setDescription(nvl(st.nextToken()));
          }
        }
      } finally {
        in.close();
      }
    } catch (Exception e) {
      e.printStackTrace();
    }

    return commands;
  }

  private static String nvl(String str) {
    return str == null ? "" : str.equals("NULL") ? "" : str;
  }
}
