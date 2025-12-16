import { HTMLAttributes } from 'react';

declare function ElAutocomplete(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElCommandPalette(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElCommandList(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDefaults(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElNoResults(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElCommandGroup(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElCommandPreview(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDialog(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDialogPanel(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDialogBackdrop(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDisclosure(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElSelect(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElSelectedcontent(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElMenu(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElDropdown(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElOptions(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElOption(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElPopover(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElPopoverGroup(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElTabList(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElTabPanels(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare function ElTabGroup(props: HTMLAttributes<HTMLElement> & {
    ref?: any;
}): any;
declare module 'react' {
    interface ButtonHTMLAttributes<T> {
        command?: 'show-modal' | 'close' | 'request-close' | 'show-popover' | 'hide-popover' | 'toggle-popover' | `--${string}` | undefined;
        commandfor?: string | undefined;
    }
}
declare module 'react' {
    interface ButtonHTMLAttributes<T> {
        command?: 'show-modal' | 'close' | 'request-close' | 'show-popover' | 'hide-popover' | 'toggle-popover' | '--show' | '--hide' | '--toggle' | `--${string}` | undefined;
        commandfor?: string | undefined;
    }
}

export { ElAutocomplete, ElCommandGroup, ElCommandList, ElCommandPalette, ElCommandPreview, ElDefaults, ElDialog, ElDialogBackdrop, ElDialogPanel, ElDisclosure, ElDropdown, ElMenu, ElNoResults, ElOption, ElOptions, ElPopover, ElPopoverGroup, ElSelect, ElSelectedcontent, ElTabGroup, ElTabList, ElTabPanels };
