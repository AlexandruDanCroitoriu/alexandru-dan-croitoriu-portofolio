# Auto-Increment Order for TemplateFolder

## Implementation Summary

A helper method `getNextFolderOrder()` has been added to `StylusSession` to automatically assign the next available order number when creating new `TemplateFolder` objects.

### What was changed:

1. **StylusSession.h**: Added public method declaration
   ```cpp
   int getNextFolderOrder();
   ```

2. **StylusSession.cpp**: Implemented the method
   - Queries all existing folders
   - Finds the maximum order value
   - Returns `maxOrder + 1`
   - First folder gets order 1 if no folders exist

3. **StylusSession.cpp**: Updated `createInitialData()` to set order for initial folders

## How to Use

When creating a new `TemplateFolder`, use the helper method:

```cpp
// Example in RootNode::createNewFolderDialog() or wherever you create folders
Wt::Dbo::Transaction t(session_);

// Get the next available order number
int nextOrder = session_.getNextFolderOrder();

// Create new folder with auto-incremented order
auto newFolder = session_.addNew<TemplateFolder>();
newFolder.modify()->folderName_ = "New Folder Name";
newFolder.modify()->order = nextOrder;
newFolder.modify()->expanded_ = true;

t.commit();
```

## Database Behavior

- Order starts at **1** for the first folder
- Subsequent folders get **2, 3, 4**, etc.
- If you delete a folder and create a new one, it gets the next available number (no gaps)
- Order is stored in the `order_index` column of the `template_folder` table

## Example: Complete Implementation in FolderNode

When implementing `FolderNode::createNewFolderDialog()`:

```cpp
void FolderNode::createNewFolderDialog()
{
    wApp->log("debug") << "FolderNode::createNewFolderDialog";
    
    auto dialog = Wt::WApplication::instance()->root()->addChild(
        std::make_unique<Wt::WDialog>("Create new folder"));
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    
    auto container = dialog->contents();
    
    // Add input field for folder name
    auto input = container->addNew<Wt::WLineEdit>();
    input->setPlaceholderText("Folder name");
    
    // Add buttons
    auto okButton = container->addNew<Wt::WPushButton>("OK");
    auto cancelButton = container->addNew<Wt::WPushButton>("Cancel");
    
    okButton->clicked().connect([=]() {
        std::string folderName = input->text().toUTF8();
        
        if (!folderName.empty()) {
            Wt::Dbo::Transaction t(session_);
            
            // Get next order number and create folder
            int nextOrder = session_.getNextFolderOrder();
            auto newFolder = session_.addNew<TemplateFolder>();
            newFolder.modify()->folderName_ = folderName;
            newFolder.modify()->order = nextOrder;
            newFolder.modify()->expanded_ = true;
            
            t.commit();
            
            // Refresh the UI tree
            // (emit signal or refresh the parent tree view)
            folders_changed_.emit(folderName);
        }
        
        dialog->done(Wt::DialogCode::Accepted);
    });
    
    cancelButton->clicked().connect([=]() {
        dialog->done(Wt::DialogCode::Rejected);
    });
    
    dialog->show();
}
```

## Notes

- The method is **transaction-aware** - you'll typically call it within a `Wt::Dbo::Transaction`
- It iterates through existing folders in memory, so it's efficient for reasonable folder counts
- For large datasets (1000+ folders), consider a SQL-based approach using `max()` aggregate function
- The order field is used for UI positioning/sorting of folders in the list
