/***************************************************************************
                         qgslocatorfilter.h
                         ------------------
    begin                : May 2017
    copyright            : (C) 2017 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSLOCATORFILTER_H
#define QGSLOCATORFILTER_H

#include "qgis_core.h"
#include "qgslocatorcontext.h"
#include "qgslogger.h"
#include <QString>
#include <QVariant>
#include <QIcon>

class QgsFeedback;
class QgsLocatorFilter;

/**
 * \class QgsLocatorResult
 * \ingroup core
 * Encapsulates properties of an individual matching result found by a QgsLocatorFilter.
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsLocatorResult
{
  public:

    /**
     * Constructor for QgsLocatorResult.
     */
    QgsLocatorResult() = default;

    /**
     * Constructor for QgsLocatorResult.
     */
    QgsLocatorResult( QgsLocatorFilter *filter, const QString &displayString, const QVariant &userData = QVariant() )
      : filter( filter )
      , displayString( displayString )
      , userData( userData )
    {}

    /**
     * Filter from which the result was obtained. This is automatically set.
     */
    QgsLocatorFilter *filter = nullptr;

    /**
     * String displayed for result.
     */
    QString displayString;

    /**
     * Descriptive text for result.
     */
    QString description;

    /**
     * Custom reference or other data set by the filter.
     */
    QVariant userData;

    /**
     * Icon for result.
     */
    QIcon icon;

    /**
     * Match score, from 0 - 1, where 1 represents a perfect match.
     */
    double score = 0.5;

};

/**
 * \class QgsLocatorFilter
 * \ingroup core
 * Abstract base class for filters which collect locator results.
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsLocatorFilter : public QObject
{
    Q_OBJECT

  public:

    //! Filter priority. Controls the order of results in the locator.
    enum Priority
    {
      Highest, //!< Highest priority
      High, //!< High priority
      Medium, //!< Medium priority
      Low, //!< Low priority
      Lowest //!< Lowest priority
    };

    //! Flags for locator behavior.
    enum Flag
    {
      FlagFast = 1 << 1, //!< Filter finds results quickly and can be safely run in the main thread
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    /**
     * Constructor for QgsLocatorFilter.
     */
    QgsLocatorFilter( QObject *parent = nullptr );

    /**
     * Creates a clone of the filter. New requests are always executed in a
     * clone of the original filter.
     */
    virtual QgsLocatorFilter *clone() const = 0 SIP_FACTORY;

    /**
     * Returns the unique name for the filter. This should be an untranslated string identifying the filter.
     * \see displayName()
     */
    virtual QString name() const = 0;

    /**
     * Returns a translated, user-friendly name for the filter.
     * \see name()
     */
    virtual QString displayName() const = 0;

    /**
     * Returns flags which specify the filter's behavior.
     */
    virtual QgsLocatorFilter::Flags flags() const;

    /**
     * Returns the priority for the filter, which controls how results are
     * ordered in the locator.
     */
    virtual Priority priority() const { return Medium; }

    /**
     * Returns the search prefix character(s) for this filter. Prefix a search
     * with these characters will restrict the locator search to only include
     * results from this filter.
     * \note Plugins are not permitted to utilize prefixes with < 3 characters,
     * as these are reserved for core QGIS functions. If a plugin registers
     * a filter with a prefix shorter than 3 characters then the prefix will
     * be ignored.
     * \note Prefixes might be overridden by user preferences.
     * \see activePrefix()
     */
    virtual QString prefix() const { return QString(); }

    /**
     * Prepares the filter instance for an upcoming search for the specified \a string. This method is always called
     * from the main thread, and individual filter subclasses should perform whatever
     * tasks are required in order to allow a subsequent search to safely execute
     * on a background thread.
     */
    virtual void prepare( const QString &string, const QgsLocatorContext &context ) { Q_UNUSED( string ); Q_UNUSED( context ); }

    /**
     * Retrieves the filter results for a specified search \a string. The \a context
     * argument encapsulates the context relating to the search (such as a map
     * extent to prioritize).
     *
     * Implementations of fetchResults() should emit the resultFetched()
     * signal whenever they encounter a matching result.
     *
     * Subclasses should periodically check the \a feedback object to determine
     * whether the query has been canceled. If so, the subclass should return
     * from this method as soon as possible.
     *
     * This will be called from a background thread unless flags() returns the
     * QgsLocatorFilter::FlagFast flag.
     */
    virtual void fetchResults( const QString &string, const QgsLocatorContext &context, QgsFeedback *feedback ) = 0;

    /**
     * Triggers a filter \a result from this filter. This is called when
     * one of the results obtained by a call to fetchResults() is triggered
     * by a user. The filter subclass must implement logic here
     * to perform the desired operation for the search result.
     * E.g. a file search filter would open file associated with the triggered
     * result.
     */
    virtual void triggerResult( const QgsLocatorResult &result ) = 0;

    /**
     * Returns true if the filter should be used when no prefix
     * is entered.
     * \see setUseWithoutPrefix()
     */
    bool useWithoutPrefix() const;

    /**
     * Sets whether the filter should be used when no prefix
     * is entered.
     * \see useWithoutPrefix()
     */
    void setUseWithoutPrefix( bool useWithoutPrefix );

    /**
     * Returns the prefix in use in the locator
     * is entered.
     * \see setActivePrefix()
     * \since QGIS 3.2
     */
    QString activePrefix() const;

    /**
     * Sets the prefix as being used by the locator
     * \see activePrefix()
     * \note If activePrefix is empty, no prefix is used. If activePrefix is NULL, the default prefix is used.
     * \since QGIS 3.2
     */
    void setActivePrefix( const QString &activePrefix ) SIP_SKIP;

    /**
     * Tests a \a candidate string to see if it should be considered a match for
     * a specified \a search string.
     * Filter subclasses should use this method when comparing strings instead
     * of directly using QString::contains() or Python 'in' checks.
     */
    static bool stringMatches( const QString &candidate, const QString &search );

    /**
     * Returns true if the filter is enabled.
     * \see setEnabled()
     */
    bool enabled() const;

    /**
     * Sets whether the filter is \a enabled.
     * \see enabled()
     */
    void setEnabled( bool enabled );

    /**
     * Should return true if the filter has a configuration widget.
     * \see createConfigWidget()
     */
    virtual bool hasConfigWidget() const;

    /**
     * Opens the configuration widget for the filter (if it has one), with the specified \a parent widget.
     * The base class implementation does nothing. Subclasses can override this to show their own
     * custom configuration widget.
     * \note hasConfigWidget() must return true to indicate that the filter supports configuration.
     */
    virtual void openConfigWidget( QWidget *parent = nullptr );

  signals:

    /**
     * Emitted when the filter finishes fetching results.
     */
    void finished();

    /**
     * Should be emitted by filters whenever they encounter a matching result
     * during within their fetchResults() implementation.
     */
    void resultFetched( const QgsLocatorResult &result );

  private:

    bool mEnabled = true;
    bool mUseWithoutPrefix = true;
    QString mActivePrefifx = QString();

};

Q_DECLARE_METATYPE( QgsLocatorResult )
Q_DECLARE_OPERATORS_FOR_FLAGS( QgsLocatorFilter::Flags )


#endif // QGSLOCATORFILTER_H


