TEMPLATE = subdirs

SUBDIRS = crengine/crengine \
    cr3-kindle/src/device \
    cr3-kindle/src

arm {
    SUBDIRS += drivers/QKindleFb \
    drivers/KindleTS \
    drivers/KindleKeyboard
}

CONFIG += ordered
