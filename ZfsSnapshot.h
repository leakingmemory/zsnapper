//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_ZFSSNAPSHOT_H
#define ZSNAPPER_ZFSSNAPSHOT_H

#include <string>
#include <memory>

class ZfsSnapshotImpl;

class ZfsSnapshot {
private:
    std::shared_ptr<ZfsSnapshotImpl> impl;
public:
    ZfsSnapshot() = default;
    ZfsSnapshot(const std::string &dataset, const std::string &snapshot);
    [[nodiscard]] std::string GetSnapshotName() const;
    [[nodiscard]] std::string GetSnapshorDirectory() const;
};


#endif //ZSNAPPER_ZFSSNAPSHOT_H
