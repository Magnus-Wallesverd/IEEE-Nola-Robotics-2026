-- Vim Settings

-- Automatically wraps text that extends beyond the screen length
vim.cmd("set wrap")

-- Sets line numbers to be relative
vim.cmd("set number")

-- Status Bar Configs
vim.cmd("set laststatus=2")
vim.cmd("set showmode")

-- Indenting configs
vim.cmd("set autoindent")
vim.cmd("set expandtab")
vim.cmd("set tabstop=4")
vim.cmd("set shiftwidth=4")

-- Escape insert and visual mode with kj 
-- vim.cmd("inoremap kj <Esc>")
-- vim.cmd("vnoremap kj <Esc>")

-- Leader mapping
vim.g.mapleader = " "
vim.g.maplocalleader = " "

-- Quit 
vim.keymap.set("n", "<leader>q", ":q<CR>")

-- Write
vim.keymap.set("n", "<leader>w", ":w<CR>")

-- Split panes
vim.keymap.set("n", "<leader>s", ":split<CR>")
vim.keymap.set("n", "<leader>v", ":vsplit<CR>")

-- Tabs
vim.keymap.set("n", "<leader>nt", ":tabnew<CR>")

-- Moving panes
vim.keymap.set("n", "C-h", ":wincmd h<CR>")
vim.keymap.set("n", "C-j", ":wincmd j<CR>")
vim.keymap.set("n", "C-k", ":wincmd k<CR>")
vim.keymap.set("n", "C-l", ":wincmd l<CR>")

-- Make Commands
vim.keymap.set("n", "<leader>m", ":!make<CR>")
vim.keymap.set("n", "<leader>mc", ":!make clean && make<CR>")
vim.keymap.set("n", "<leader>md", ":!make dump<CR>")
vim.keymap.set("n", "<leader>md", ":!make dump<CR>")
vim.keymap.set("n", "<leader>mf", ":!make flash<CR>")
