#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WStringStream.h>
#include <Wt/WSignal.h>

/**
 * @brief A Monaco code editor widget integrated with Wt
 * 
 * MonacoEditor provides a rich code editor with syntax highlighting, 
 * customizable themes, and various editor features like line wrapping,
 * minimap toggle, and file operations.
 */
class MonacoEditor : public Wt::WContainerWidget {
public:
    /**
     * @brief Constructor - creates a Monaco editor for the specified language
     * @param language Programming language for syntax highlighting (e.g., "javascript", "css", "html")
     */
    MonacoEditor(std::string language);
    
    /**
     * @brief Sets the read-only state of the editor
     * @param readOnly True to make editor read-only, false to allow editing
     */
    void setReadOnly(bool readOnly);
    
    /**
     * @brief Checks if there are unsaved changes in the editor
     * @return True if there are unsaved changes, false otherwise
     */
    bool unsavedChanges();
    
    /**
     * @brief Gets the current unsaved text content
     * @return String containing the unsaved text
     */
    std::string getUnsavedText() { return unsaved_text_; }
    
    /**
     * @brief Gets the current saved text content
     * @return String containing the saved text
     */
    std::string getSavedText() { return current_text_; }
    
    /**
     * @brief Marks the current text as saved (synchronizes current and unsaved text)
     */
    void textSaved();
    
    /**
     * @brief Sets editor content directly via JavaScript (avoids resource registration)
     * @param content The text content to set in the editor
     */
    void setContent(const std::string& content);
    
    /**
     * @brief Saves the current editor content to the selected file
     */
    void saveFile();
    
    /**
     * @brief Sets line wrapping in the editor
     * @param wrap True to enable line wrapping, false to disable
     */
    void setLineWrap(bool wrap);
    
    /**
     * @brief Toggles the minimap display in the editor
     */
    void toggleMinimap();
    
    /**
     * @brief Sets the line number display
     * @param show True to show line numbers, false to hide them
     */
    void setLineNumber(bool show);
    
    /**
     * @brief Resets the editor layout (useful after size changes)
     */
    void resetLayout();
    
    /**
     * @brief Sets the global dark theme for all Monaco editors
     * @param dark True for dark theme, false for light theme
     */
    static void setDarkTheme(bool dark);
    
    /**
     * @brief Reads text content from a file
     * @param filePath Path to the file to read
     * @return File content as string
     */
    static std::string getFileText(std::string filePath);

    /**
     * @brief Signal emitted when save operation is requested
     * @return Signal that provides the text content to save
     */
    Wt::Signal<std::string>& saveFileSignal() { return save_file_signal_; }
    
    /**
     * @brief Signal emitted when save operation becomes available
     * @return Signal emitted when unsaved changes exist
     */
    Wt::Signal<>& availableSave() { return available_save_; }
    
    /**
     * @brief Signal emitted when editor width changes
     * @return Signal that provides the new width
     */
    Wt::Signal<Wt::WString>& widthChanged() { return width_changed_; }
protected:
    /**
     * @brief Called when the widget size changes
     * @param width New width in pixels
     * @param height New height in pixels
     */
    void layoutSizeChanged(int width, int height) override;
        
private:
    /**
     * @brief Callback for when editor text content changes
     * @param text New text content
     */
    void editorTextChanged(std::string text);

    std::string selected_file_path_;       ///< Path to currently selected file
    std::string current_text_;             ///< Current saved text content
    std::string unsaved_text_;             ///< Unsaved text content
    std::string editor_js_var_name_;       ///< JavaScript variable name for this editor instance
    
    Wt::JSignal<std::string> js_signal_text_changed_;  ///< JavaScript signal for text changes
    Wt::Signal<> available_save_;                       ///< Signal for save availability
    Wt::Signal<std::string> save_file_signal_;          ///< Signal for save file operation
    Wt::Signal<Wt::WString> width_changed_;             ///< Signal for width changes
};