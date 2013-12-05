/*
 * Copyright (C) 2012 Victor Pyankov & Sergey Baranov
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

import com.amazon.kindle.kindlet.AbstractKindlet;
import com.amazon.kindle.kindlet.KindletContext;
import com.amazon.kindle.kindlet.ui.KButton;
import com.amazon.kindle.kindlet.ui.KPanel;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;
import java.util.List;

/**
 * Created Jan 20, 2012 1:35:11 PM
 *
 * @author Victor Pyankov & Serge Baranov
 */
public abstract class RunnerKindlet extends AbstractKindlet implements CommandExecuter {
  private KindletContext ctx;

  public void create(KindletContext context) {
    ctx = context;
  }

  public void showUI(CommandLoader loader) {
    Container rootContainer = ctx.getRootContainer();
    try {
      List commands = loader.load();

      // default to CoolReader 3
      if (commands.isEmpty()) {
        commands.add(new Command("/mnt/us/cr3", "goqt.sh", "cr3", "Cool Reader 3"));
      }

      if (commands.size() == 1) {
        execute((Command) commands.get(0));
        return;
      }

      KPanel btnPanel = new KPanel();
      GridLayout layout = new GridLayout(commands.size(), 1);
      layout.setVgap(2);
      btnPanel.setLayout(layout);

      for (int i = 0; i < commands.size(); i++) {
        Command command = (Command) commands.get(i);
        final KButton btn = new KButton(command.getDescription());
        btn.setFont(new Font(btn.getFont().getName(), Font.BOLD, btn.getFont().getSize() + 6));
        command.setExecuter(this);
        btn.addKeyListener(command);
        // flash button
        btn.addKeyListener(new KeyListener() {
          public void keyTyped(KeyEvent e) {
          }

          public void keyPressed(KeyEvent e) {
            btn.setBackground(Color.BLACK);
            btn.repaint();
          }

          public void keyReleased(KeyEvent e) {
            btn.setBackground(Color.WHITE);
            btn.repaint();
          }
        });
        btnPanel.add(btn);
      }

      rootContainer.add(btnPanel, BorderLayout.NORTH);

    } catch (Throwable t) {
      rootContainer.repaint();
    }
  }

  public void execute(Command command) {
    try {
      Runtime runtime = Runtime.getRuntime();

      runtime.exec(new String[]{command.getPath() + File.separatorChar +
                                command.getCommand(), command.getParam()}, null, new File(command.getPath()));

    } catch (Throwable ignored) {}
  }
}
