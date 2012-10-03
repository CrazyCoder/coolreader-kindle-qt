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
package kz.pvnhome.kindle;

import kz.pvnhome.cr3runner.Command;
import kz.pvnhome.cr3runner.CommandExecuter;
import kz.pvnhome.cr3runner.CommandLoader;

import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.List;

public class TestApp extends Frame implements CommandExecuter {
  private static final long serialVersionUID = -9030497265581541689L;

  private final Label label;

  public TestApp() {
    super("TestApp");

    setSize(200, 200);

    CommandLoader loader = new CommandLoader("conf/cr3runner", "commands.txt");
    List commands = loader.load();

    Panel btnPanel = new Panel();
    btnPanel.setLayout(new GridLayout(commands.size(), 1));

    for (int i = 0; i < commands.size(); i++) {
      Command command = (Command) commands.get(i);
      Button btn = new Button(command.getDescription());
      command.setExecuter(this);
      btn.addKeyListener(command);
      btn.addActionListener(command);
      btnPanel.add(btn);
    }

    add(btnPanel, BorderLayout.NORTH);

    label = new Label("Ready...");
    add(label, BorderLayout.CENTER);

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        System.out.println("windowClosing");
        setVisible(false);
        dispose();
        System.exit(0);
      }
    });
  }

  public static void main(String[] args) {
    System.out.println("start");
    TestApp app = new TestApp();
    app.setVisible(true);
    System.out.println("stop");
  }

  public void execute(Command command) {
    System.out.println("execute:" + command.getDescription());
    label.setText(command.getDescription());
  }
}
