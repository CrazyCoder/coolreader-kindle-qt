TEMPLATE = subdirs

SUBDIRS = crengine/crengine \
    cr3-kindle/src/device \
    cr3-kindle/src \
    drivers/KindleKeyboard

arm {
    SUBDIRS += drivers/QKindleFb \
    drivers/KindleTS
}

CONFIG += ordered
