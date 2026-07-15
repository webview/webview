from pathlib import Path
from playwright.sync_api import sync_playwright, expect


root = Path(__file__).resolve().parents[1]
output = root / "artifacts" / "qa"
output.mkdir(parents=True, exist_ok=True)

with sync_playwright() as playwright:
    browser = playwright.chromium.launch(headless=True)
    page = browser.new_page(viewport={"width": 1440, "height": 900}, device_scale_factor=1)
    errors = []
    page.on("console", lambda message: errors.append(message.text) if message.type == "error" else None)
    page.on("pageerror", lambda error: errors.append(str(error)))

    page.goto("http://127.0.0.1:5173/#/dashboard")
    page.wait_for_load_state("networkidle")
    expect(page.get_by_role("heading", name="下午好，林清越")).to_be_visible()
    page.screenshot(path=str(output / "dashboard-1440.png"), full_page=True)

    page.get_by_test_id("sidebar-toggle").click()
    expect(page.locator(".app-sidebar")).to_have_class("app-sidebar is-collapsed")

    page.get_by_role("button", name="切换语言").click()
    page.get_by_text("English", exact=True).click()
    expect(page.get_by_text("Dashboard", exact=True).first).to_be_visible()

    page.get_by_role("button", name="Change theme").click()
    page.get_by_text("Dark", exact=True).click()
    expect(page.locator("html")).to_have_class("app-dark")
    expect(page.get_by_role("button", name="Minimize")).to_be_visible()
    expect(page.get_by_role("button", name="Maximize")).to_be_visible()
    expect(page.get_by_role("button", name="Close window")).to_be_visible()

    for width in (1280, 1024):
        page.set_viewport_size({"width": width, "height": 800})
        page.wait_for_timeout(120)
        overflow = page.evaluate("document.documentElement.scrollWidth > document.documentElement.clientWidth")
        assert not overflow, f"Horizontal overflow at {width}px"
        page.screenshot(path=str(output / f"dashboard-{width}.png"), full_page=True)

    for route in ("projects", "tasks", "files", "analytics", "team", "messages", "calendar", "assistant", "profile", "settings"):
        page.goto(f"http://127.0.0.1:5173/#/{route}")
        page.wait_for_load_state("networkidle")
        assert page.locator("main").count() == 1

    assert not errors, "Browser console errors: " + " | ".join(errors)
    browser.close()
