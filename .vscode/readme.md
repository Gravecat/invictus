# .vscode

"What's this VSCode folder doing in the repo? Shouldn't it be on .gitignore?" I hear you ask.

Not so! The reason it's here -- aside from VSCode being my primary editor while working on Morior Invictus -- is because of the `editor.rulers` rule.
You can ignore `files.associations` (it's just used internally and doesn't really affect anything meaningful), but I've explicitly set a ruler for a width of
160 characters.

Why 160? Because I *really* dislike long code lines that are forced to wrap around in an editor (or worse, that stretch out the scrollbar on a web browser
when browsing code online), and 160 seemed like a good value that looks good in the editor and looks good on GitHub on my screen. It's a largely arbitrary
number, but it does the job. :3

Obviously, anyone contributing to this project don't have to use VSCode if they don't want to, but I'd at least ask folks to please consider keeping lines no
longer than 160 characters.
