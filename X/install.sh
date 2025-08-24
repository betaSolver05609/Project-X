#!/usr/bin/env bash
# ===============================
# Project X Installer (Portable)
# ===============================

set -e  # Exit on error

# --- Self-heal CRLF issue ---
if file "$0" | grep -q "CRLF"; then
    echo "[fix] Converting CRLF to LF line endings..."
    if command -v dos2unix >/dev/null 2>&1; then
        dos2unix "$0"
    else
        sed -i 's/\r$//' "$0"
    fi
    exec "$0" "$@"   # restart script after fix
fi

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
    echo "Unsupported package manager. Please install g++, make, and jsoncpp manually."
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

# JSONCPP library (dev headers)
if ! ls /usr/include/jsoncpp/json/json.h >/dev/null 2>&1 && \
   ! ls /usr/include/json/json.h >/dev/null 2>&1; then
    echo "Installing jsoncpp headers..."
    $INSTALL_CMD libjsoncpp-dev || $INSTALL_CMD jsoncpp-devel || true
else
    echo "✔ jsoncpp headers found"
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
INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

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
