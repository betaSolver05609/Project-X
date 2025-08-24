#!/usr/bin/env bash

set -e  # Exit on error

PROJECT_NAME="projectx"
BUILD_BINARY="db_start"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"   # This will be ProjectX/X

echo "[1/4] Checking package manager..."

# Detect package manager
if command -v apt-get >/dev/null 2>&1; then
    INSTALL_CMD="sudo apt-get install -y"
elif command -v dnf >/dev/null 2>&1; then
    INSTALL_CMD="sudo dnf install -y"
elif command -v yum >/dev/null 2>&1; then
    INSTALL_CMD="sudo yum install -y"
else
    echo "Unsupported package manager. Please install g++, make, and libjsoncpp manually."
    exit 1
fi

echo "[2/4] Checking dependencies..."

# Required tools
for pkg in g++ make; do
    if ! command -v $pkg >/dev/null 2>&1; then
        echo "Installing $pkg..."
        $INSTALL_CMD $pkg
    else
        echo "✔ $pkg found"
    fi
done

# JSONCPP library
if ! ldconfig -p | grep -q libjsoncpp; then
    echo "Installing libjsoncpp-dev..."
    $INSTALL_CMD libjsoncpp-dev
else
    echo "✔ libjsoncpp found"
fi

echo "[3/4] Building project in $ROOT_DIR ..."
cd "$ROOT_DIR"
make clean
make

if [ ! -f "$BUILD_BINARY" ]; then
    echo "❌ Build failed. Exiting."
    exit 1
fi

echo "[4/4] Installing launcher..."

# Install path (prefer ~/.local/bin if available)
if [ -d "$HOME/.local/bin" ] || mkdir -p "$HOME/.local/bin"; then
    INSTALL_DIR="/usr/local/bin"
else
    INSTALL_DIR="/usr/local/bin"
fi

# Wrapper script that always runs from ProjectX/X
cat <<EOF > "$INSTALL_DIR/$PROJECT_NAME"
#!/usr/bin/env bash
"$ROOT_DIR/$BUILD_BINARY" "\$@"
EOF

chmod +x "$INSTALL_DIR/$PROJECT_NAME"

echo
echo "✅ Installation complete!"
echo "You can now run the database by typing: $PROJECT_NAME"
echo
echo "⚡ If command not found, add this to your ~/.bashrc or ~/.zshrc:"
echo "   export PATH=\$PATH:$INSTALL_DIR"
