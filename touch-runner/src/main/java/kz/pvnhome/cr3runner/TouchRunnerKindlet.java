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
import com.amazon.kindle.kindlet.ui.KOptionPane;
import ixtab.jailbreak.Jailbreak;
import ixtab.jailbreak.SuicidalKindlet;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.security.AllPermission;
import java.util.List;

/**
 * Kindle Touch version
 *
 * @author Victor Pyankov & Serge Baranov
 */
public class TouchRunnerKindlet extends SuicidalKindlet implements CommandExecuter {
  private static final String CONFIG_DIR  = "/mnt/us/touchrunner";
  private static final String CONFIG_FILE = "commands.txt";

  private KindletContext ctx;
  private Container      rootContainer;

  protected void onCreate(KindletContext context) {
    ctx = context;

    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        checkJailBreak();
      }
    });
  }

  private void checkJailBreak() {
    if (jailbreak.isAvailable()) {
      if (((LocalJailbreak) jailbreak).requestPermissions()) {
        ctx.getRootContainer().removeAll();
        initCommandsUI();
      } else {
        String title = "Kindlet Jailbreak Failed";
        String error = "The Kindlet Jailbreak failed to obtain all required permissions. Please report this error.";
        KOptionPane.showMessageDialog(ctx.getRootContainer(), error, title);
      }
    } else {
      String title = "Kindlet Jailbreak Required";
      String message =
        "This application requires the Kindlet Jailbreak to be installed. This is an additional jailbreak that must be installed on top of the Device Jailbreak, in order to allow Kindlets to get the required permissions. Please install the Kindlet Jailbreak before using this application.";
      KOptionPane.showMessageDialog(ctx.getRootContainer(), message, title);
    }
  }

  private void initCommandsUI() {
    try {
      rootContainer = ctx.getRootContainer();

      CommandLoader loader = new CommandLoader(CONFIG_DIR, CONFIG_FILE);
      List commands = loader.load();

      // default to CoolReader 3
      if (commands.isEmpty()) {
        commands.add(new Command("/mnt/us/cr3", "goqt.sh", "cr3", "Cool Reader 3"));
      }

      if (commands.size() == 1) {
        execute((Command) commands.get(0));
        return;
      }

      JPanel btnPanel = new JPanel();
      btnPanel.setLayout(new GridLayout(commands.size(), 1));

      for (int i = 0; i < commands.size(); i++) {
        final Command command = (Command) commands.get(i);
        JButton btn = new JButton(command.getDescription());
        btn.setFont(new Font(btn.getFont().getName(), Font.BOLD, btn.getFont().getSize() + 6));
        command.setExecuter(this);
        btn.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            execute(command);
          }
        });
        btnPanel.add(btn);
      }

      rootContainer.add(btnPanel, BorderLayout.NORTH);
      rootContainer.validate();
    } catch (Throwable ignored) {
      rootContainer.repaint();
    }
  }

  public void execute(Command command) {
    try {
      Runtime runtime = Runtime.getRuntime();

      runtime.exec(new String[]{command.getPath() + File.separatorChar +
                                command.getCommand(), command.getParam()}, null, new File(command.getPath()));

    } catch (Throwable ignored) {
      rootContainer.repaint();
    }
  }

  protected Jailbreak instantiateJailbreak() {
    return new LocalJailbreak();
  }

  private static class LocalJailbreak extends Jailbreak {
    public boolean requestPermissions() {
      return getContext().requestPermission(new AllPermission());
    }
  }
}
