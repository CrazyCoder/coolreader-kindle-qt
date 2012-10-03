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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

/**
 * Created Jan 22, 2012 1:34:20 PM
 *
 * @author Victor Pyankov
 */
public class Command implements KeyListener, ActionListener {
  private String          path;
  private String          command;
  private String          param;
  private String          description;
  private CommandExecuter executer;

  public Command() {
  }

  public Command(String path, String command, String param, String description) {
    this.path = path;
    this.command = command;
    this.param = param;
    this.description = description;
  }

  public String getPath() {
    return path;
  }

  public void setPath(String path) {
    this.path = path;
  }

  public String getCommand() {
    return command;
  }

  public void setCommand(String command) {
    this.command = command;
  }

  public String getParam() {
    return param;
  }

  public void setParam(String param) {
    this.param = param;
  }

  public String getDescription() {
    return description;
  }

  public void setDescription(String description) {
    this.description = description;
  }

  public void setExecuter(CommandExecuter executer) {
    this.executer = executer;
  }

  //======================================================
  // KeyListener
  //======================================================

  public void keyPressed(KeyEvent e) {
  }

  public void keyReleased(KeyEvent e) {
    System.out.println("keyReleased");
    if (executer != null) {
      executer.execute(this);
    }
  }

  public void keyTyped(KeyEvent e) {
  }

  //======================================================
  // ActionListener
  //======================================================

  public void actionPerformed(ActionEvent e) {
    System.out.println("actionPerformed");
    if (executer != null) {
      executer.execute(this);
    }
  }
}
