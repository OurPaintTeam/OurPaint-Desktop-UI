#ifndef OURPAINT_MENUTYPES_H
#define OURPAINT_MENUTYPES_H
namespace UI {
    enum class tabMenuType {
    deleteTab,
    renameTab,
    closeTab,
    closeAllTabs,
    closeAllRightTabs,
    closeAllLeftTabs,
    closeOtherTabs,
};

    enum class version {
        init,
        create,
        push,
        pull,
        commit,
    };


    enum class collab {
        open,
        close,
    };


    enum class projectMenuType {
        createFile,
        exportFile,
        openFile,
        scriptFile,
    };
}
#endif //OURPAINT_MENUTYPES_H
