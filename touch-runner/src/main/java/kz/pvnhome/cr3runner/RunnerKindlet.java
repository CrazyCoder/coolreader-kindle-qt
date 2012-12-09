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
 * Date: 11/6/12 Time: 4:54 PM
 *
 * @author serge
 */
public abstract class RunnerKindlet extends SuicidalKindlet implements CommandExecuter {
  protected void onCreate(final KindletContext context) {
    new Thread() {
      public void run() {
        try {
          while (!(context.getRootContainer().isValid() && context.getRootContainer().isVisible())) {
            Thread.sleep(100);
          }
        } catch (Exception ignored) {}
        SwingUtilities.invokeLater(new Runnable() {
          public void run() {
            checkJailBreak(context);
          }
        });
      }
    }.start();
  }

  private void checkJailBreak(KindletContext ctx) {
    if (jailbreak.isAvailable()) {
      if (((LocalJailbreak) jailbreak).requestPermissions()) {
        ctx.getRootContainer().removeAll();
        initCommandsUI(ctx);
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

  protected abstract void initCommandsUI(KindletContext ctx);

  public void execute(Command command) {
    try {
      Runtime runtime = Runtime.getRuntime();

      runtime.exec(new String[]{command.getPath() + File.separatorChar +
                                command.getCommand(), command.getParam()}, null, new File(command.getPath()));

    } catch (Throwable ignored) {}
  }

  protected Jailbreak instantiateJailbreak() {
    return new LocalJailbreak();
  }

  protected void showUI(KindletContext ctx, CommandLoader loader) {
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

  private static class LocalJailbreak extends Jailbreak {
    public boolean requestPermissions() {
      return getContext().requestPermission(new AllPermission());
    }
  }
}
