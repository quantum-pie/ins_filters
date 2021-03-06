#ifndef VAR_H
#define VAR_H

#include "communication/terminalbase.h"

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <QObject>

/*!
 * @brief The Var class
 * This class is designed to encaplsulate terminal variable.
 */
class Var
{
public:
    /*!
     * @brief Var constructor.
     * @param tbase TerminalBase reference.
     */
    Var(TerminalBase & tbase) : tbase{ tbase }
    {}

    /*!
     * @brief Set variable.
     * @tparam T type of variable value.
     * @param name Variable name.
     * @param val Variable value.
     */
    template<typename T>
    void set(const std::string & name, const T & val)
    {
        tbase.send_text(name + ' ' + boost::lexical_cast<std::string>(val) + '\n');
    }

    /*!
     * @brief Get variable value.
     * @tparam T type of variable value.
     * @param name Variable name.
     * @return variable value.
     */
    template<typename T>
    T get(const std::string & name)
    {
        bool ready { false };
        T result {};

        auto temporary_connection = QObject::connect(&tbase, &TerminalBase::text_received,
        [&](const std::string & input)
        {
            std::vector<std::string> tokens;
            boost::split(tokens, input, boost::is_any_of(" \n"));
            result = boost::lexical_cast<T>(tokens[1]);
            ready = true;
        });

        tbase.send_text(name + '\n');

        auto t1 = std::chrono::high_resolution_clock::now();
        auto t2 = t1;

        while(!ready &&
              std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() < timeout_ms)
        {
            QCoreApplication::processEvents();
            t2 = std::chrono::high_resolution_clock::now();
        }

        QObject::disconnect(temporary_connection);
        return result;
    }

private:
    static const int timeout_ms { 200 };
    TerminalBase & tbase;
};

#endif // VAR_H
